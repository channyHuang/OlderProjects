
//
#include "multiviewstereo.hpp"

#include "camera.hpp"
#include "geodesicweight.hpp"
#include "lineiter.hpp"

#include <QtGui/QColor>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QFileInfo>

#include <iostream>
#include <fstream>

#include "../TrackCameraLib/data.h"

using namespace std;

//---------------------------------------------------------------------

using namespace Eigen;

using std::max;
using std::min;
using std::exp;
using std::make_pair;

#define PV(x, y, img)        (static_cast<int>(y)*img.width() + static_cast<int>(x))
#define CONTAINS(img, x, y)  (x >= 0 && y >= 0 && x < img.width() && y < img.height())

namespace {
	const double NaN = std::numeric_limits<double>::quiet_NaN();
	const double INF = std::numeric_limits<double>::infinity();
}

//---------------------------------------------------------------------
// allow passing these values to constructor
//
namespace {
	const int WINDOW_RADIUS = 2;
	const int WINDOW_SIZE = 2 * WINDOW_RADIUS + 1;

	const size_t NUM_NEIGHBOURING_VIEWS = 3;

	// From paper
	const int K = 1;//9;
	const double BETA = 1;
	const double LAMBDA = 1;
	const double PHIU = 0.5;
	const double PSIU = 0.002;
}

//---------------------------------------------------------------------
// better way of dealing with weight functions
//
class AdaptiveWeight;
class GeodesicWeight;
typedef GeodesicWeight WeightFunc;

//---------------------------------------------------------------------

double cost_ncc(
	const VectorImage &img1, const VectorImage &img2,
	const VectorImage &img1Mask, const VectorImage &img2Mask,
	int x1, int y1, int x2, int y2,
	const WeightFunc &weightFunc)
{
	// Find neighboudhood means
	double meanL = 0, meanR = 0;
	double totalWeight = 0.0;
	for (int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for (int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
#if 0
			if (img1Mask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if (img2Mask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			const RGBA &lrgb = img1.sample(x1 + col, y1 + row);
			//const RGBA &lrgb = img1.pixel(x1 + col, y1 + row);
			if (!lrgb.isValid()) continue;

			const RGBA &rrgb = img2.sample(x2 + col, y2 + row);
			//const RGBA &rrgb = img2.pixel(x2 + col, y2 + row);
			if (!rrgb.isValid()) continue;

			const double weight = weightFunc(row, col);
			if (weight > 1e-10) {
				meanL += weight*lrgb.toGray();
				meanR += weight*rrgb.toGray();
				totalWeight += weight;
			}
		}
	}

	if (totalWeight < 1e-10)
		return 1000;

	meanL /= totalWeight;
	meanR /= totalWeight;

	// Cross-correlation
	double sum1 = 0, sum2 = 0, sum3 = 0;
	for (int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for (int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
#if 0
			if (img1Mask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if (img2Mask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			const RGBA &lrgb = img1.sample(x1 + col, y1 + row);
			//const RGBA &lrgb = img1.pixel(x1 + col, y1 + row);
			if (!lrgb.isValid()) continue;

			const RGBA &rrgb = img2.sample(x2 + col, y2 + row);
			//const RGBA &rrgb = img2.pixel(x2 + col, y2 + row);
			if (!rrgb.isValid()) continue;

			const double weight = weightFunc(row, col);
			if (weight > 1e-10) {
				const double pixel_gray_l = weight*lrgb.toGray();
				const double pixel_gray_r = weight*rrgb.toGray();
				sum1 += (pixel_gray_l - meanL)*(pixel_gray_r - meanR);
				sum2 += (pixel_gray_l - meanL)*(pixel_gray_l - meanL);
				sum3 += (pixel_gray_r - meanR)*(pixel_gray_r - meanR);
			}
		}
	}

	//if (sum2 * sum3 < 1e-10)
	//	return 0;
	//return sum1 / std::sqrt(sum2 * sum3);

	if (120 < 255 * (1.0 - abs(sum1) / std::sqrt(sum2 * sum3))) 
		return 120;
	else return 255 * (1.0 - abs(sum1) / std::sqrt(sum2 * sum3));
}
//---------------------------------------------------------------------

void MultiViewStereo::initialize(
	const std::vector<QString> &imagenames,
	const std::vector<QString> &masknames,
	const std::vector<Camera> &views,
	double minDepth, double maxDepth,
	int numDepthLevels,
	double crossCheckThreshold,
	double imageScale)
{
	this->minDepth = minDepth;
	this->maxDepth = maxDepth;
	this->numDepthLevels = numDepthLevels;
	this->crossCheckThreshold = crossCheckThreshold;
	this->imageScale = imageScale;

	this->views.clear();
	images.clear();
	masks.clear();
	results.clear();
	computedDepths.clear();

	// Load images and masks
	for (size_t index = 0; index < views.size(); ++index) {
		//	if(views[index]) {
		if (QFileInfo(imagenames.at(index)).exists()) {
			QImage baseImage(imagenames.at(index));
			QImage image = baseImage.scaledToWidth(baseImage.width() * imageScale, Qt::SmoothTransformation);
			images.push_back(VectorImage::fromQImage(image));

			// load mask image
			if (masknames.size() != 0) {
				QImage mask(masknames.at(index));
				VectorImage mask_vector;
				masks.push_back(mask_vector.fromQImage(mask.scaledToWidth(mask.width()*imageScale, Qt::SmoothTransformation)));
			}
			else {
				masks.push_back(VectorImage(image.width(), image.height(), WHITE));
			}

			results.push_back(VectorImage(image.width(), image.height()));
			computedDepths.push_back(DepthMap(image.width()*image.height(), NaN));

			this->views.push_back(views[index]);
		}
	}

	neighbours.resize(this->views.size());
}

//---------------------------------------------------------------------

namespace {
	RGBA NAN_COLOR(255, 255, 255);
	RGBA INF_COLOR(255, 255, 255);
	RGBA UNKNOWN_COLOR(255, 255, 255);
}

RGBA MultiViewStereo::colorFromDepth(double depth) const {
	if (std::isnan(depth)) return NAN_COLOR;
	if (std::isinf(depth)) return INF_COLOR;
	if (depth + 1e-5 < minDepth) return UNKNOWN_COLOR;

	// Reduce depths to [0, 1] range
	const double t = min(1.0, max(0.0, (depth - minDepth) / (maxDepth - minDepth)));

	// Black = close, white = far
	return RGBA(255 * t);
}

//---------------------------------------------------------------------

QImage MultiViewStereo::depthMap(Camera view) const {
	for (size_t viewIndex = 0; viewIndex < views.size(); ++viewIndex) {
		if (views[viewIndex].id() == view.id())
			return VectorImage::toQImage(results[viewIndex], false);
	}
//	return QImage();
	return VectorImage::toQImage(results[0], false);
}

//---------------------------------------------------------------------
// output 3D points to .ply file
//
void outputPLYFile(const std::string &path, const std::vector<PLYPoint> &points) {
	//
	std::ofstream lout(path.c_str());
	lout << "ply\n"
		<< "format ascii 1.0\n"
		<< "element vertex " << points.size() << "\n"
		<< "property float x\n"
		<< "property float y\n"
		<< "property float z\n"
		<< "property uchar diffuse_red\n"
		<< "property uchar diffuse_green\n"
		<< "property uchar diffuse_blue\n"
		<< "end_header\n";

	foreach(const PLYPoint &pp, points) {
		const Ray3d::Point &p = pp.first;
		const RGBA &rgb = pp.second;

		lout << p[0] << ' ' << p[1] << ' ' << p[2]
			<< ' ' << static_cast<int>(rgb.r)
			<< ' ' << static_cast<int>(rgb.g)
			<< ' ' << static_cast<int>(rgb.b)
			<< '\n';
	}
}

void outputSurfel(const std::string &path, const std::vector<Surfel> &surfels)
{
	//
	std::ofstream lout(path.c_str());
	lout << "ply\n"
		<< "format ascii 1.0\n"
		<< "element vertex " << surfels.size() << "\n"
		<< "property float x\n"
		<< "property float y\n"
		<< "property float z\n"
		<< "property uchar diffuse_red\n"
		<< "property uchar diffuse_green\n"
		<< "property uchar diffuse_blue\n"
		<< "end_header\n";

	foreach(const Surfel &surfel, surfels) {
		const Ray3d::Point &p = surfel.Point;
		const RGBA &rgb = surfel.color;

		lout << p[0] << ' ' << p[1] << ' ' << p[2]
			<< ' ' << static_cast<int>(rgb.r)
			<< ' ' << static_cast<int>(rgb.g)
			<< ' ' << static_cast<int>(rgb.b)
			<< '\n';
	}
}


//---------------------------------------------------------------------

/*int MultiViewStereo::numSteps() const {
	return 2 * (int)views.size(); // initial estimate + cross-checking;
}
*/
//---------------------------------------------------------------------

void MultiViewStereo::runTask() {
	int current_step = 0;

	//  Compute neighbouring images. Take the NUM_NEIGHBOURING_VIEWS closest
	//  cameras that have a reasonably small angle with the current view.

	for (size_t view_index = 0; view_index < views.size(); ++view_index) {
		std::vector< std::pair<double, size_t> > nearViews;

		Camera cam1 = views[view_index];
		for (size_t viewIndex2 = 0; viewIndex2 < views.size(); ++viewIndex2) {
			if (view_index != viewIndex2) {
				Camera cam2 = views[viewIndex2];

				// If angle between viewing directions is less than some threshold
				if (fabs(cam1.principleRay().direction().dot(cam2.principleRay().direction())) > 0.2) {
					double dist = (cam1.C() - cam2.C()).squaredNorm();
					nearViews.push_back(make_pair(dist, viewIndex2));
				}
			}
		}

		auto end = nearViews.end();
		if (NUM_NEIGHBOURING_VIEWS < nearViews.size()) {
			std::sort(nearViews.begin(), nearViews.end());
			end = nearViews.begin() + NUM_NEIGHBOURING_VIEWS;
		}

		neighbours[view_index].clear();
		for (auto iter = nearViews.begin(); iter != end; ++iter)
			neighbours[view_index].push_back(iter->second);
	}

	//
	// Compute initial stereo estimate
	//
	for (size_t view_index = 0; view_index < views.size(); ++view_index) {
		computeInitialEstimate(view_index);

		//refine max and min value of depth
		double maxn = 0, minn = INF;
		for (int y = 0; y < results[view_index].height(); ++y) {
			for (int x = 0; x < results[view_index].width(); ++x) {
				if (isnan(computedDepths[view_index][PV(x, y, results[view_index])]) || isinf(computedDepths[view_index][PV(x, y, results[view_index])]))
					continue;
				double d = computedDepths[view_index][PV(x, y, results[view_index])];
				if (maxn < d) maxn = d;
				if (minn > d) minn = d;
			}
		}

		cout << "Depth; max - min: " << maxn << " - " << minn << endl;
		minDepth = minn;
		maxDepth = maxn;
		// Construct resulting depth map from computed depths
		for (int y = 0; y < results[view_index].height(); ++y) {
			for (int x = 0; x < results[view_index].width(); ++x) {
				if (masks[view_index].pixel(x, y) == WHITE) {
					const double d = computedDepths[view_index][PV(x, y, results[view_index])];
					results[view_index].setPixel(x, y, colorFromDepth(d));
				}
				else {
					results[view_index].setPixel(x, y, BLACK);
				}
			}
		}

		VectorImage::toQImage(results[view_index], false).save(DATA_PATH + QString::number(view_index) + QString(".jpg"));
	}

	// Compute percentage of pixels (in mask) that have a depth hypothesis
	for (size_t view_index = 0; view_index < views.size(); ++view_index) {	
		int total = 0, have = 0;
		for (int y = 0; y < results[view_index].height(); ++y) {
			for (int x = 0; x < results[view_index].width(); ++x) {
				if (masks[view_index].pixel(x, y) == WHITE) {
					++total;
					const double d = computedDepths[view_index][PV(x, y, results[view_index])];
					if (std::isfinite(d))
						++have;
				}
			}
		}
		qDebug() << QString("%1:").arg(views[view_index].name())
			<< ((100.0*have) / total) << "percent of pixels have depth hypotheses (before cross-check)";
	}


	// Cross-checking
	for (size_t view_index = 0; view_index < views.size(); ++view_index) {
		crossCheck(view_index);
	}

	QString plyname = DATA_PATH + QString("ThreeDScene.ply");
	outputPLYFile(plyname.toStdString(), pointcloud);
}

//---------------------------------------------------------------------
// K determined by index in vector, PeakPair = <NCC cost, depth>
typedef std::pair<double, double> PeakPair;

struct CostFunction {
	static std::vector<std::vector<std::vector<PeakPair> > > peakPairs;
	static int width, height;
};

std::vector<std::vector<std::vector<PeakPair> > > CostFunction::peakPairs;
int CostFunction::width = 0;
int CostFunction::height = 0;

//
typedef std::pair<double, PLYPoint> PointPair;

struct PointFunction {
	static std::vector<std::vector<std::vector<PointPair> > > pointPairs;
	static int width, height;
};
std::vector<std::vector<std::vector<PointPair> > >PointFunction::pointPairs;
int PointFunction::width = 0;
int PointFunction::height = 0;

bool compare(PointPair a, PointPair b)
{
	return a.first < b.first;
}

//---------------------------------------------------------------------

void MultiViewStereo::computeInitialEstimate(size_t viewIndex) {
	Camera preview = views[0];
	//---

	const int width = images[viewIndex].width();
	const int height = images[viewIndex].height();
	const Ray3d::Point &cameraC = views[viewIndex].C();
	const Ray3d::Vector &depthPlaneNormal = views[viewIndex].principleRay().direction();

	Camera view = views[viewIndex];
	const VectorImage &image = images[viewIndex];
	const VectorImage &mask = masks[viewIndex];

	//
	CostFunction::peakPairs.clear();
	CostFunction::peakPairs.resize(height);
	//
	PointFunction::pointPairs.clear();
	PointFunction::pointPairs.resize(height);

	for (int y = 0; y < height; ++y) {
		CostFunction::peakPairs[y].resize(width);

		PointFunction::pointPairs[y].resize(width);
	}
	// First, for each pixel, collect the top K NCC peaks from surrounding
	// views which we will later feed to an MRF optimization routine

	Eigen::Vector3d threeDp;

	{
		WeightFunc weightFunc(WINDOW_RADIUS);
		for (int y = 0; y < height; ++y) {

			for (int x = 0; x < width; ++x) {
				//
				if (masks[viewIndex].pixel(x, y) != WHITE)
					continue;

				computedDepths[viewIndex][PV(x, y, results[viewIndex])] = INF;

				std::vector<PeakPair> &peaks = CostFunction::peakPairs[y][x];
				peaks.resize(K, PeakPair(0, -1));

				std::vector<PointPair> &points = PointFunction::pointPairs[y][x];

				PLYPoint initplypoint(Eigen::Vector3d(0, 0, 0), RGBA(0, 255, 0));
				points.resize(K, PointPair(0.0, initplypoint));
				

				// Allow weighting function to initialize anything window-specific
				//weightFunc.init_weights(images[viewIndex], x, y);
				weightFunc.initialize(5);

				// Multi-view NCC cost collection
				Ray3d ray = view.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);

				foreach(size_t viewIndex2, neighbours[viewIndex]) {
					const Camera &oview = views[viewIndex2];
					const VectorImage &oimage = images[viewIndex2];
					const VectorImage &omask = masks[viewIndex2];

					std::vector<Eigen::Vector3d> curve;

					for (int ix = (x - width / 10.0 > 0 ? x - width / 10.0 : 0); ix < width && ix < x + width / 10.0; ix++) {
						if (omask.pixel(ix, y) != WHITE) continue;
						curve.push_back(Vector3d(ix, y, 1.0));
					}

					// Sample the ray, project samples into right view to obtain
					// a piece-wise linear estimate of the epipolar curve. Find
					// NCC scores along that curve

					foreach(const Eigen::Vector3d &p, curve) {
						Ray3d ray2 = oview.unproject((p[0] + 0.5) / imageScale, (p[1] + 0.5) / imageScale);
						Eigen::Vector3d p1, p2;
						ray.closestPoints(ray2, p1, p2);

						const double cost = cost_ncc(image, oimage, mask, omask, x, y, p[0], p[1], weightFunc);
					
						if (cost > 0.95) {
							p1 += p2;
							p1 *= 0.5;
							p1 = view.fromGlobalToLocal(p1);
							peaks.push_back(PeakPair(cost, p1.z()));
							//
							threeDp = view.fromLocalToGlobal(p1);
							threeDp = preview.fromGlobalToLocal(threeDp);
							
							points.push_back(PointPair(cost, PLYPoint(threeDp, image.pixel(x, y))));
						}
					}
				}

				// Take the top K peaks. Since a larger NCC score is better
				// than a small one, take the last K peaks after sorting.
				std::sort(peaks.begin(), peaks.end());
				//peaks.erase(peaks.begin(), peaks.end() - K);
				peaks = std::vector<PeakPair>(peaks.end() - K, peaks.end());


				std::sort(points.begin(), points.end(), compare);
				points = std::vector<PointPair>(points.end() - K, points.end());

				/*
				std::vector<PeakPair> &peaks = CostFunction::peakPairs[y][x];
				std::vector<PointPair> &points = PointFunction::pointPairs[y][x];
				if (masks[viewIndex].pixel(x, y) != WHITE)
					continue;
				// Allow weighting function to initialize anything window-specific
				weightFunc.init_weights(images[viewIndex], x, y);

				// Multi-view NCC cost collection
				Ray3d ray = view.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);
				{
					size_t viewIndex2 = viewIndex + 1;
					if (viewIndex2 > views.size()) viewIndex2 = 0;
					const Camera &oview = views[viewIndex2];
					const VectorImage &oimage = images[viewIndex2];
					const VectorImage &omask = masks[viewIndex2];

					std::vector<Eigen::Vector3d> curve;
					for (int ix = 0; ix < width; ix++) {
						if (omask.pixel(ix, y) != WHITE) continue;
						curve.push_back(Vector3d(ix, y, 1.0));
					}

					double secondBestCost = INF;
					double minCost = INF;
					// Sample the ray, project samples into right view to obtain
					// a piece-wise linear estimate of the epipolar curve. Find
					// NCC scores along that curve
					foreach(const Eigen::Vector3d &p, curve) {
						Ray3d ray2 = oview.unproject((p[0] + 0.5) / imageScale, (p[1] + 0.5) / imageScale);
						Eigen::Vector3d p1, p2;
						ray.closestPoints(ray2, p1, p2);

						const double cost = cost_ncc(image, oimage, mask, omask, x, y, p[0], p[1], weightFunc);
						if (cost + 1e-10 < minCost) {
							p1 += p2;
							p1 *= 0.5;
							p1 = view.fromGlobalToLocal(p1);

							secondBestCost = minCost;
							minCost = cost;
							peaks.push_back(PeakPair(cost, p1.z()));

							threeDp = p1;
						}
					}
					const double cmincost = minCost;
					points.push_back(PointPair(cmincost, PLYPoint(threeDp, image.pixel(x, y))));
				}
	*/			
			}
		}
	}

	

	for (int y = 0, p = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x, ++p) {
			if (PointFunction::pointPairs[y][x].size() == 0 || CostFunction::peakPairs[y][x].size() == 0) continue;
			if (mask.pixel(x, y) == WHITE) {
				computedDepths[viewIndex][p] = CostFunction::peakPairs[y][x].back().second;

				//add points to 3D scenes	
				pointcloud.push_back(PointFunction::pointPairs[y][x].back().second);


				//////////////////////////////////////////////////////////////////////////
				//mysurfels.surfelOpeLine(views[viewIndex], PointFunction::pointPairs[y][x].back().second.first, images[viewIndex].pixel(x, y), PointFunction::pointPairs[y][x].back().first);
			}
		}
	}


	
}

//---------------------------------------------------------------------

void MultiViewStereo::crossCheck(size_t view_index) {
	Camera view = views[view_index];
	Ray3d::Vector viewNormal = view.principleRay().direction();

	// Invalidate pixels that fail to cross-check

	for (int y = 0; y < images[view_index].height(); ++y) {
		for (int x = 0; x < images[view_index].width(); ++x) {
			//
			double &depth = computedDepths[view_index][PV(x, y, results[view_index])];
			if (!std::isfinite(depth))
				continue;

			//
			Ray3d ray = view.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);
			Ray3d::Point p1 = view.C();
			if (pointFromDepth(ray, viewNormal, depth, p1)) {
				double x2, y2;
				bool found = false;
				for (size_t view_index2 = 0; view_index2 < views.size(); ++view_index2) {
					if (view_index2 == view_index)
						continue;

					Camera oview = views[view_index2];
					if (oview.project(p1, x2, y2)) {
						x2 = x2*imageScale;
						y2 = y2*imageScale;

						if (CONTAINS(results[view_index2], x2, y2)) {
							double &odepth = computedDepths[view_index2][PV(x2, y2, results[view_index2])];
							if (std::isfinite(odepth)) {
								Ray3d ray2 = oview.unproject((x2 + 0.5) / imageScale, (y2 + 0.5) / imageScale);
								Ray3d::Point p2 = oview.C();
								Ray3d::Vector oviewNormal = oview.principleRay().direction();
								if (pointFromDepth(ray2, oviewNormal, odepth, p2)) {
									const double norm = (p1 - p2).norm();
									if (std::isfinite(norm) && norm < crossCheckThreshold) {
										found = true;
										break;
									}
								}
							}
						}
					}
				}

				if (!found)
					depth = NaN;
			}
		}
	}
}

//---------------------------------------------------------------------

double MultiViewStereo::depthFromLabel(int label) const {
	double t = label / (numDepthLevels - 1.0);
	return minDepth*(1 - t) + maxDepth*t;
}

//---------------------------------------------------------------------

bool MultiViewStereo::pointFromDepth(
	const Ray3d &ray,
	const Ray3d::Vector &normal,
	double depth,
	Ray3d::Point &p) const
{
	Plane3d plane(normal, p + normal*depth);
	return intersect(ray, plane, p);
}
