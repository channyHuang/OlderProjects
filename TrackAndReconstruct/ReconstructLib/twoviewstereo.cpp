//---------------------------------------------------------------------

#include "twoviewstereo.hpp"

#include "geodesicweight.hpp"
#include "lineiter.hpp"

#include "rectification.hpp"

#include "stdlib.h"
#include "time.h"

#include <QtGui/QColor>
#include <QtCore/QDebug>
#include <QtCore/QTime>

#include <fstream>
#include <iostream>
using namespace std;

#include "multiviewstereo.hpp"

#undef USE_MRF

#   define omp_get_num_procs() 1
#   define omp_get_thread_num() 0

#define PV(x, y, img)        (static_cast<int>(y)*img.width() + static_cast<int>(x))
#define CONTAINS(img, x, y)  (x >= 0 && y >= 0 && x < img.width() && y < img.height())

namespace {
	const double NaN = std::numeric_limits<double>::quiet_NaN();
	const double INF = std::numeric_limits<double>::infinity();
}

namespace {
	const int BAD_RET = 1000;
	const int WINDOW_RADIUS = 5;
	const int WINDOW_SIZE = 2 * WINDOW_RADIUS + 1;

	const int    SMOOTHNESS_EXP = 1;
	const double SMOOTHNESS_MAX = 2;
	const double SMOOTHNESS_LAMBDA = 0.25;

	const int    GAP_WIDTH_THRESHOLD = 30;//2;//100*imageScale;
	const double MAX_COLOR_DIFF = 120;
	const double INCONSISTENCY_THRESH = 1;
	const double SECOND_BEST_FACTOR = 0.95;
}

const int NUM_WEIGHT_FUNCS = 16;
typedef GeodesicWeight WeightFunc;
static WeightFunc weightFuncs[NUM_WEIGHT_FUNCS];

//---------------------------------------------------------------------

TwoViewStereo::TwoViewStereo(
		Camera leftView, QImage left, QImage leftMask,
		Camera rightView, QImage right, QImage rightMask,
		double minDepth, double maxDepth,
		int numDepthLevels,
		double imageScale)
	: leftView(leftView)
	, rightView(rightView)
	, left( VectorImage::fromQImage(left.scaledToWidth(left.width() * imageScale, Qt::SmoothTransformation)) )
	, right( VectorImage::fromQImage(right.scaledToWidth(right.width() * imageScale, Qt::SmoothTransformation)) )
	, minDepth(minDepth)
	, maxDepth(maxDepth)
	, numDepthLevels(numDepthLevels)
	, imageScale(imageScale)
{
	// Images
	if(!leftMask.isNull())
		this->leftMask = VectorImage::fromQImage(leftMask.scaledToWidth(leftMask.width() * imageScale, Qt::SmoothTransformation));
	else
		this->leftMask = VectorImage(this->left.width(), this->left.height(), WHITE);

	if(!rightMask.isNull())
		this->rightMask = VectorImage::fromQImage(rightMask.scaledToWidth(rightMask.width() * imageScale, Qt::SmoothTransformation));
	else
		this->rightMask = VectorImage(this->right.width(), this->right.height(), WHITE);
	
	// Results
	resultLeft = VectorImage(this->left.width(), this->left.height());
	resultRight = VectorImage(this->right.width(), this->right.height());
	computedDepthLeft = DepthMap(this->left.width()*this->left.height(), NaN);
	computedDepthRight = DepthMap(this->left.width()*this->left.height(), NaN);
	
	// Weight funcs
	for(int i = 0; i < NUM_WEIGHT_FUNCS; ++i)
		weightFuncs[i].initialize(WINDOW_RADIUS);

//	paramTransRectification(leftView, rightView, left, right);
}

//---------------------------------------------------------------------

RGBA TwoViewStereo::colorFromDepth(double depth) const {
	if(std::isnan(depth))
		return BLACK;

	if(std::isinf(depth))
		return BLACK;

	// For grayscale depth maps
	const double t = min(1.0, max(0.0, (depth - minDepth) / (maxDepth - minDepth)));
//	return RGBA(255*(1 - t));
	return RGBA(255 * t);
}

//---------------------------------------------------------------------

void TwoViewStereo::computeDepthMaps() {
	//
	resultLeft.fill(INVALID);
	resultRight.fill(INVALID);

	threeDpoints.clear();
	//
	computeCostVolumes(leftView, rightView);	
#if 0
		for(int y = 0; y < resultLeft.height(); ++y) {
			for(int x = 0; x < resultLeft.width(); ++x) {
				double d = computedDepthLeft[ PV(x, y, resultLeft) ];
				resultLeft.setPixel(x, y, colorFromDepth(d));
			}
		}

		for (int y = 0; y < resultRight.height(); ++y) {
			for (int x = 0; x < resultRight.width(); ++x) {
				double d = computedDepthRight[PV(x, y, resultRight)];
				resultRight.setPixel(x, y, colorFromDepth(d));
			}
		}
#endif
#if 0	
	crossCheck(leftView, rightView);
	{
		for(int y = 0; y < resultLeft.height(); ++y) {
			for(int x = 0; x < resultLeft.width(); ++x) {
				double d = computedDepthLeft[ PV(x, y, resultLeft) ];
				resultLeft.setPixel(x, y, colorFromDepth(d));
			}
		}

		for(int y = 0; y < resultRight.height(); ++y) {
			for(int x = 0; x < resultRight.width(); ++x) {
				double d = computedDepthRight[ PV(x, y, resultRight) ];
				resultRight.setPixel(x, y, colorFromDepth(d));
			}
		}
	}
#endif

	QDateTime time = QDateTime::currentDateTime();
	QString timename = time.toString("dd_hh_mm_ss");

	outputPLYFile((timename + QString("twoview3dpoint.ply")).toStdString(), threeDpoints);

	//outputSurfel((timename + QString("twoviewsurfel.ply")).toStdString(), mysurfels.surfels);
}


//---------------------------------------------------------------------

void TwoViewStereo::computeCostVolumes(Camera leftView, Camera rightView) {
	srand((unsigned)time(NULL));

#if 1 //compute left image
		Ray3d::Vector depthPlaneNormal = leftView.principleRay().direction();

		QTime time = QTime::currentTime();

		const Ray3d::Point cameraC = leftView.C();
#pragma omp parallel for
		for(int y = 0; y < left.height(); ++y) {

			vector<Eigen::Vector3d> matchpoints;
			//----------------------------------------------------
			for(int x = 0; x < left.width(); ++x) {
				computedDepthLeft[PV(x, y, resultLeft)] = NaN;

				if (leftMask.pixel(x, y) != WHITE)
					continue;
				//----------------------------------------------------
				std::vector<Eigen::Vector3d> curve;
				//if matching points are too far away, ignore it
				//now temporary define the search area in this:
				/*for (int ix = (x - left.width() / 10.0 > 0 ? x - left.width() / 10.0 : 0); ix < left.width() && ix < x + left.width() / 10.0; ix++) {
					if (rightMask.pixel(ix, y) != WHITE) continue;
					
					curve.push_back(Vector3d(ix, y, 1.0));
				}*/
				//add all points in line to curve
				for (int ix = 0; ix < left.width(); ix++) {
					if (rightMask.pixel(ix, y) != WHITE) continue;
					curve.push_back(Vector3d(ix, y, 1.0));
				}
				if (curve.size() == 0) continue;

				weightFuncs[omp_get_thread_num()].init_weights(left, x, y);

				Ray3d ray = leftView.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);


				double secondBestCost = INF;
				double minCost = INF;

				Eigen::Vector3d threeDp;
				//save the matching point in curve
				Eigen::Vector3d matchc;

				foreach(const Eigen::Vector3d &p, curve) {
					

					Ray3d ray2 = rightView.unproject((p[0] + 0.5) / imageScale, (p[1] + 0.5) / imageScale);

					Eigen::Vector3d p1, p2;
					ray.closestPoints(ray2, p1, p2);

					const double cost = cost_ncc(left, right, leftMask, rightMask, x, y, p[0], p[1]);
					if(cost + 1e-10 < minCost) {
						p1 += p2;
						p1 *= 0.5;
						p1 = leftView.fromGlobalToLocal(p1);		

						secondBestCost = minCost;
						minCost = cost;
						computedDepthLeft[ PV(x, y, resultLeft) ] = p1.z();

						threeDp = p1;

						matchc = p;
					}
				}

				threeDpoints.push_back(PLYPoint(threeDp, left.pixel(x, y)));
				/*
				if(minCost > SECOND_BEST_FACTOR*secondBestCost)
					computedDepthLeft[ PV(x, y, resultLeft) ] = INF;
					*/

				//////////////////////////////////////////////////////////////////////////
				//mysurfels.surfelOpeLine(leftView, threeDp, left.pixel(x, y), minCost);


				//save matching points
				if (rand() % 7 == 0) {
 					Eigen::Vector3d matchp(y, x, matchc[0]);
					matchpoints.push_back(matchp);
				}
			}
#if 0
			//save the matching images
			cv::Mat canvas = limg.clone();
			cv::resize(canvas, canvas, cv::Size(left.width(), left.height() * 2));
			cv::Mat roiImage = canvas(cv::Rect(0, 0, left.width(), left.height()));
			resize(limg, limg, cv::Size(left.width(), left.height()));
			cv::addWeighted(roiImage, 0.0, limg, 1.0, 0.0, roiImage);
			roiImage = canvas(cv::Rect(0, left.height(), left.width(), left.height()));
			resize(rimg, rimg, cv::Size(left.width(), left.height()));
			cv::addWeighted(roiImage, 0.0, rimg, 1.0, 0.0, roiImage);
			foreach(const Eigen::Vector3d &p, matchpoints) {
				line(canvas, cv::Point(p[1], p[0]), cv::Point(p[2], p[0] + left.height()), cv::Scalar(0, 0, 255));
				//cv::imshow("canvas", canvas);
				//cv::waitKey(100);
			}
			char matchimgname[20];
			sprintf(matchimgname, "F:/%d.jpg", y);
			imwrite(matchimgname, canvas);
#endif // DEBUG
		}
		cout << '\t' << time.elapsed() / 1000.0 << "seconds";

		//----------------------------------------------------
		double maxn = 0, minn = INF;
		for (int y = 0; y < left.height(); ++y) {
			for (int x = 0; x < left.width(); ++x) {
				if (isnan(computedDepthLeft[PV(x, y, resultLeft)]) || isinf(computedDepthLeft[PV(x, y, resultLeft)]))
					continue;
				maxn = max(computedDepthLeft[PV(x, y, resultLeft)], maxn);
				minn = min(computedDepthLeft[PV(x, y, resultLeft)], minn);
			}
		}
		cout << "(left image) Depth: max - min:" << maxn << "-" << minn << endl;
		minDepth = minn;
		maxDepth = maxn;

		for (int y = 0; y < resultLeft.height(); ++y) {
			for (int x = 0; x < resultLeft.width(); ++x) {
				double d = computedDepthLeft[PV(x, y, resultLeft)];
				resultLeft.setPixel(x, y, colorFromDepth(d));
			}
		}
#endif

#if 0 //the same as above, just compute the right image
	{
		Ray3d::Vector depthPlaneNormal = rightView.principleRay().direction();

		QTime time = QTime::currentTime();

		const Ray3d::Point &cameraC = rightView.C();

		#pragma omp parallel for
		for(int y = 0; y < right.height(); ++y) {
			
			for(int x = 0; x < right.width(); ++x) {
				computedDepthRight[PV(x, y, resultRight)] = NaN;
				if (rightMask.pixel(x, y) != WHITE)
					continue;

				//----------------------------------------------------
				std::vector<Eigen::Vector3d> curve;
				//if matching points are too far away, ignore it
				for (int ix = (x - right.width() / 10.0 > 0 ? x - right.width() / 10.0 : 0); ix < right.width() && ix < x + right.width() / 10.0; ix++) {
					if (leftMask.pixel(ix, y) != WHITE) continue;
					

					curve.push_back(Vector3d(ix, y, 1.0));
				}
				/*//add all points in line to curve
				for (int ix = 0; ix < left.width(); ix++) {
					if (rightMask.pixel(ix, y) != WHITE) continue;
					curve.push_back(Vector3d(ix, y, 1.0));
				}*/

				if (curve.size() == 0) continue;
				//----------------------------------------------------

				
				

				weightFuncs[omp_get_thread_num()].init_weights(right, x, y);

				Ray3d ray = rightView.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);

				double secondBestCost = INF;
				double minCost = INF;

				Eigen::Vector3d threeDp;

				foreach(const Eigen::Vector3d &p, curve) {
					Ray3d ray2 = leftView.unproject((p[0] + 0.5) / imageScale, (p[1] + 0.5) / imageScale);

					Eigen::Vector3d p1, p2;
					ray.closestPoints(ray2, p1, p2);

					const double cost = cost_ncc(right, left, rightMask, leftMask, x, y, p[0], p[1]);
					if(cost + 1e-10 < minCost) {
						p1 += p2;
						p1 *= 0.5;
						p1 = rightView.fromGlobalToLocal(p1);

						secondBestCost = minCost;
						minCost = cost;
						computedDepthRight[ PV(x, y, resultRight) ] = p1.z();

						threeDp = p1;
					}
				}
				//translate the 3D point from right image axis to left image
				threeDp = rightView.fromLocalToGlobal(threeDp);
				threeDp = leftView.fromGlobalToLocal(threeDp);

				threeDpoints.push_back(PLYPoint(threeDp, right.pixel(x, y)));
				/*if(minCost > SECOND_BEST_FACTOR*secondBestCost)
					computedDepthRight[ PV(x, y, resultRight) ] = INF;*/


				//////////////////////////////////////////////////////////////////////////
				//mysurfels.surfelOpeLine(leftView, threeDp, left.pixel(x, y), minCost);
			}
		}
		cout << '\t' << time.elapsed() / 1000.0 << "seconds";


		//----------------------------------------------------
		double maxn = 0, minn = INF;
		for (int y = 0; y < right.height(); ++y) {
			for (int x = 0; x < right.width(); ++x) {
				if (isnan(computedDepthRight[PV(x, y, resultRight)]) || isinf(computedDepthRight[PV(x, y, resultRight)]))
					continue;
				maxn = max(computedDepthRight[PV(x, y, resultRight)], maxn);
				minn = min(computedDepthRight[PV(x, y, resultRight)], minn);
			}
		}
		cout << "(right image) Depth: max - min:" << maxn << "-" << minn << endl;
		minDepth = minn;
		maxDepth = maxn;

		for (int y = 0; y < resultRight.height(); ++y) {
			for (int x = 0; x < resultRight.width(); ++x) {
				double d = computedDepthRight[PV(x, y, resultRight)];
				resultRight.setPixel(x, y, colorFromDepth(d));
			}
		}
	}
#endif
}

//---------------------------------------------------------------------

void TwoViewStereo::crossCheck(Camera leftView, Camera rightView) {
	{
		Ray3d::Vector leftPlaneNormal = leftView.principleRay().direction();
		Ray3d::Vector rightPlaneNormal = rightView.principleRay().direction();

		// Invalidate pixels that fail to cross-check
		for(int y = 0; y < left.height(); ++y) {
			for(int x = 0; x < left.width(); ++x) {
				//
				double &depth = computedDepthLeft[ PV(x, y, resultLeft) ];
				if(!std::isfinite(depth))
					continue;

				//
				Ray3d ray = leftView.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);
				Ray3d::Point p1 = leftView.C();
				if(pointFromDepth(ray, leftPlaneNormal, depth, p1)) {
					double x2, y2;
					if(rightView.project(p1, x2, y2)) {
						x2 = x2*imageScale;
						y2 = y2*imageScale;

						if(CONTAINS(resultRight, x2, y2)) {
							double &odepth = computedDepthRight[ PV(x2, y2, resultRight) ];
							if(std::isfinite(odepth)) {
								Ray3d ray2 = rightView.unproject((x2 + 0.5) / imageScale, (y2 + 0.5) / imageScale);
								Ray3d::Point p2 = rightView.C();
								if(pointFromDepth(ray2, rightPlaneNormal, odepth, p2)) {
									const double norm = (p1 - p2).norm();
									if(!std::isfinite(norm) || norm > INCONSISTENCY_THRESH)
										depth = INF;
								} else depth = INF;
							} else depth = INF;
						} else depth = INF;
					} else depth = INF;
				}
			}
		}

		for(int y = 0; y < right.height(); ++y) {
			for(int x = 0; x < right.width(); ++x) {
				//
				double &depth = computedDepthRight[ PV(x, y, resultRight) ];
				if(!std::isfinite(depth))
					continue;

				//
				Ray3d ray = rightView.unproject((x + 0.5) / imageScale, (y + 0.5) / imageScale);
				Ray3d::Point p1 = rightView.C();
				if(pointFromDepth(ray, rightPlaneNormal, depth, p1)) {
					double x2, y2;
					if(leftView.project(p1, x2, y2)) {
						x2 = x2*imageScale;
						y2 = y2*imageScale;

						if(CONTAINS(resultLeft, x2, y2)) {
							double &odepth = computedDepthLeft[ PV(x2, y2, resultLeft) ];
							if(std::isfinite(odepth)) {
								Ray3d ray2 = leftView.unproject((x2 + 0.5) / imageScale, (y2 + 0.5) / imageScale);
								Ray3d::Point p2 = leftView.C();
								if(pointFromDepth(ray2, leftPlaneNormal, odepth, p2)) {
									const double norm = (p1 - p2).norm();
									if(!std::isfinite(norm) || norm > INCONSISTENCY_THRESH)
										depth = INF;
								} else depth = INF;
							} else depth = INF;
						} else depth = INF;
					} else depth = INF;
				}
			}
		}
	}
}

//---------------------------------------------------------------------

void TwoViewStereo::filterInvalidPixels() {
	DepthMap cdLeftCopy = computedDepthLeft;
	DepthMap cdRightCopy = computedDepthRight;

	{
		for(int y = 0; y < left.height(); ++y) {
			for(int x = 0; x < left.width();) {
				double ldepth = computedDepthLeft[ PV(x, y, resultLeft) ];
				while(x < left.width() && !std::isinf(computedDepthLeft[ PV(x, y, resultLeft) ])) {
					ldepth = computedDepthLeft[ PV(x, y, resultLeft) ];
					++x;
				}

				if(x >= left.width())
					continue;

				double rdepth = INF;
				int start = x;

				while(x < left.width() && std::isinf(computedDepthLeft[ PV(x, y, resultLeft) ])) {
					rdepth = computedDepthLeft[ PV(x, y, resultLeft) ];
					++x;
				}

				// If we didn't go past the end of the image, get the depth
				// at the point where we found a non-NaN value.
				if(x < left.width())
					rdepth = computedDepthLeft[ PV(x, y, resultLeft) ];
				else
					rdepth = NaN;

				// Fill the gap, if necessary
				int end = x - 1;
				if(end - start < GAP_WIDTH_THRESHOLD) {
					if(!std::isfinite(ldepth)) ldepth = rdepth;
					if(!std::isfinite(rdepth)) rdepth = ldepth;

					while(start <= end) {
						computedDepthLeft[ PV(start, y, resultLeft) ] = ldepth;
						computedDepthLeft[ PV(end, y, resultLeft) ] = rdepth;
						++start;
						--end;
					}
				}
			}
		}

		for(int y = 0; y < right.height(); ++y) {
			for(int x = 0; x < right.width();) {
				double ldepth = computedDepthRight[ PV(x, y, resultRight) ];
				while(x < right.width() && !std::isinf(computedDepthRight[ PV(x, y, resultRight) ])) {
					ldepth = computedDepthRight[ PV(x, y, resultRight) ];
					++x;
				}

				if(x >= right.width())
					continue;

				double rdepth = INF;
				int start = x;

				while(x < right.width() && std::isinf(computedDepthRight[ PV(x, y, resultRight) ])) {
					rdepth = computedDepthRight[ PV(x, y, resultRight) ];
					++x;
				}

				// If we didn't go past the end of the image, get the depth
				// at the point where we found a non-NaN value.
				if(x < left.width())
					rdepth = computedDepthRight[ PV(x, y, resultRight) ];
				else
					rdepth = NaN;

				// Fill the gap, if necessary
				int end = x - 1;
				if(end - start < GAP_WIDTH_THRESHOLD) {
					if(!std::isfinite(ldepth)) ldepth = rdepth;
					if(!std::isfinite(rdepth)) rdepth = ldepth;

					while(start <= end) {
						computedDepthRight[ PV(start, y, resultRight) ] = ldepth;
						computedDepthRight[ PV(end, y, resultRight) ] = rdepth;
						++start;
						--end;
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------

typedef std::pair<double, double> ValuePair;

bool comparePairFirst(const ValuePair &a, const ValuePair &b) {
	return (a.first < b.first);
};

double TwoViewStereo::weightedMedian(const DepthMap &depths, const VectorImage &mask, int x, int y) {
	//
	std::vector<ValuePair> vals;
	vals.reserve(WINDOW_SIZE * WINDOW_SIZE);

	//
	double totalWeights = 0.0;
	for(int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for(int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
			const int xt = x + col;
			const int yt = y + row;
			const double depth = depths[ PV(xt, yt, mask) ];
			if(std::isnan(depth) || depth < minDepth || depth > maxDepth)
				continue;

			const double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if(weight > 1e-10) {
				vals.push_back(ValuePair(depth, weight));
				totalWeights += weight;
			}
		}
	}
	
	//
	double ret = NaN;
	if(vals.size() > 1 && totalWeights > 1e-10) {
		std::make_heap(vals.begin(), vals.end(), comparePairFirst);

		//
		double weight1 = 0.0;
		while(weight1 < totalWeights) {
			std::pop_heap(vals.begin(), vals.end(), comparePairFirst);
			weight1 += vals.back().second;
			totalWeights -= vals.back().second;
			ret = vals.back().first;
			vals.pop_back();
		}
	}
	return ret;
}

//---------------------------------------------------------------------

double TwoViewStereo::cost_sad(
		const VectorImage &left, const VectorImage &right,
		const VectorImage &leftMask, const VectorImage &rightMask,
		int x1, int y1, int x2, int y2)
{
	int numPixels = 0;
	double sum = 0.0, totalWeight = 0.0;
	for(int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for(int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
#if 1
			if(leftMask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if(rightMask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			//const RGB &lrgb = left.pixel(x1 + col, y1 + row);
			const RGBA &lrgb = left.sample(x1 + col, y1 + row);
			if(!lrgb.isValid()) continue;

			RGBA rrgb = right.pixel(x2 + col, y2 + row);
			if(!rrgb.isValid()) continue;

			double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if(weight > 1e-10) {
				//rrgb -= lrgb;
				//double diff = sqrt(rrgb.r*rrgb.r + rrgb.g*rrgb.g + rrgb.b*rrgb.b);
				//double diff = (fabs(rrgb.r) + fabs(rrgb.g) + fabs(rrgb.b)) / 3.0;
				double diff = fabs(lrgb.toGray() - rrgb.toGray());

				sum += weight*min(MAX_COLOR_DIFF, diff);
				totalWeight += weight;
				++numPixels;
			}
		}
	}

	if(numPixels <= 4 || totalWeight <= 1e-10)
		return BAD_RET;
	return (sum / totalWeight);
}

//---------------------------------------------------------------------

double TwoViewStereo::cost_ncc(
		const VectorImage &left, const VectorImage &right,
		const VectorImage &leftMask, const VectorImage &rightMask,
		int x1, int y1, int x2, int y2)
{
	// Find neighboudhood means
	double meanL = 0, meanR = 0;
	double totalWeight = 0.0;
	for(int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for(int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
#if 1
			if(leftMask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if(rightMask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			const RGBA &lrgb = left.sample(x1 + col, y1 + row);
			if(!lrgb.isValid()) continue;

			const RGBA &rrgb = right.sample(x2 + col, y2 + row);
			if(!rrgb.isValid()) continue;

			const double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if(weight > 1e-10) {
				meanL += weight*lrgb.toGray();
				meanR += weight*rrgb.toGray();
				totalWeight += weight;
			}
		}
	}

	if(totalWeight < 1e-10)
		return BAD_RET;

	meanL /= totalWeight;
	meanR /= totalWeight;

	// Cross-correlation
	double sum1 = 0, sum2 = 0, sum3 = 0;
	for(int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for(int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
			const RGBA &lrgb = left.sample(x1 + col, y1 + row);
			const RGBA &rrgb = right.sample(x2 + col, y2 + row);
#if 1
			if(leftMask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if(rightMask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			if(!lrgb.isValid()) continue;
			if(!rrgb.isValid()) continue;

			const double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if(weight > 1e-10) {
				const double pixel_gray_l = weight*lrgb.toGray();
				const double pixel_gray_r = weight*rrgb.toGray();
				sum1 += (pixel_gray_l - meanL)*(pixel_gray_r - meanR);
				sum2 += (pixel_gray_l - meanL)*(pixel_gray_l - meanL);
				sum3 += (pixel_gray_r - meanR)*(pixel_gray_r - meanR);
			}
		}
	}

	return min(MAX_COLOR_DIFF, 255*(1.0 - abs(sum1) / std::sqrt(sum2 * sum3)));
}
//---------------------------------------------------------------------
//Add cost zssd
double TwoViewStereo::cost_zssd(
	const VectorImage &left, const VectorImage &right,
	const VectorImage &leftMask, const VectorImage &rightMask, int x1, int y1, int x2, int y2)
{
	// Find neighboudhood means
	double meanL = 0, meanR = 0;
	double totalWeight = 0.0;
	for (int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for (int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
#if 1
			if (leftMask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if (rightMask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			const RGBA &lrgb = left.sample(x1 + col, y1 + row);
			if (!lrgb.isValid()) continue;

			const RGBA &rrgb = right.sample(x2 + col, y2 + row);
			if (!rrgb.isValid()) continue;

			const double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if (weight > 1e-10) {
				meanL += weight*lrgb.toGray();
				meanR += weight*rrgb.toGray();
				totalWeight += weight;
			}
		}
	}

	if (totalWeight < 1e-10)
		return BAD_RET;

	meanL /= totalWeight;
	meanR /= totalWeight;

	// Cross-correlation
	double sum = 0.0;
	for (int row = -WINDOW_RADIUS; row <= WINDOW_RADIUS; ++row) {
		for (int col = -WINDOW_RADIUS; col <= WINDOW_RADIUS; ++col) {
			const RGBA &lrgb = left.sample(x1 + col, y1 + row);
			const RGBA &rrgb = right.sample(x2 + col, y2 + row);
#if 1
			if (leftMask.pixel(x1 + col, y1 + row) != WHITE)
				continue;

			if (rightMask.pixel(x2 + col, y2 + row) != WHITE)
				continue;
#endif
			//
			if (!lrgb.isValid()) continue;
			if (!rrgb.isValid()) continue;

			const double weight = (weightFuncs[omp_get_thread_num()])(row, col);
			if (weight > 1e-10) {
				const double pixel_gray_l = weight*lrgb.toGray();
				const double pixel_gray_r = weight*rrgb.toGray();
				double diff = (pixel_gray_l - meanL - pixel_gray_r + meanR) * (pixel_gray_l - meanL - pixel_gray_r + meanR);

				sum += weight*min(MAX_COLOR_DIFF, diff);
				totalWeight += weight;
			}
		}
	}

	return (sum / totalWeight);
}
//---------------------------------------------------------------------

double TwoViewStereo::depthFromLabel(int label) const {
	double t = label / (numDepthLevels - 1.0);
	t /= (5 - 4*t); // non-uniform sampling
	return minDepth*(1 - t) + maxDepth*t;
}

bool TwoViewStereo::pointFromDepth(
		const Ray3d &ray,
		const Ray3d::Vector &normal,
		double depth,
		Ray3d::Point &p) const
{
	Plane3d plane(normal, p + normal*depth);
	return intersect(ray, plane, p);
}