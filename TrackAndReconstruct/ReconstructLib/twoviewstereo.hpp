
#ifndef STEREO_HPP
#define STEREO_HPP

#include <vector>
using namespace std;

#include "camera.hpp"
#include "vectorimage.hpp"

#include <QtGui/QImage>
#include <QtWidgets/QApplication>

#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "surfel.h"

typedef std::pair<Ray3d::Point, RGBA> PLYPoint;

void outputPLYFile(const std::string &path, const std::vector<PLYPoint> &points);
void outputSurfel(const std::string &path, const std::vector<Surfel> &surfels);

class TwoViewStereo {
public:
	typedef std::vector<double> DepthMap;
	
public:
	TwoViewStereo(Camera leftView, QImage left, QImage leftMask,
				  Camera rightView, QImage right, QImage rightMask,
				  double minDepth, double maxDepth, int numDepthLevels,
				  double imageScale = 1.0);
	
public: // Task implementation
	QString title() const { return QApplication::translate("TwoViewStereo", "Two-View Stereo"); }
	int numSteps() const { return 8; }
	void runTask() { computeDepthMaps(); }

	//matching 
	cv::Mat limg, rimg;

	//surfel
	SurfelOperation mysurfels;

	std::vector<PLYPoint> threeDpoints;
public:
	//! .
	void computeDepthMaps();
	
	//! .
	QImage leftDepthMap() const { return VectorImage::toQImage(resultLeft); }

	//! .
	QImage rightDepthMap() const { return VectorImage::toQImage(resultRight); }

protected:
	//! .
	void computeCostVolumes(Camera leftView, Camera rightView);
	
	//! .
	void crossCheck(Camera leftView, Camera rightView);
	
	//! .
	void filterInvalidPixels();
	
	//! .
	double depthFromLabel(int label) const;
	
	//! .
	bool pointFromDepth(const Ray3d &ray,
						const Ray3d::Vector &normal,
						double depth,
						Ray3d::Point &p) const;
						
	//! .
	double cost_sad(
			const VectorImage &left, const VectorImage &right,
			const VectorImage &leftMask, const VectorImage &rightMask,
			int x1, int y1, int x2, int y2);
	//! .
	double cost_ncc(
			const VectorImage &left, const VectorImage &right,
			const VectorImage &leftMask, const VectorImage &rightMask,
			int x1, int y1, int x2, int y2);
	//! .
	//Add cost zssd
	double cost_zssd(
		const VectorImage &left, const VectorImage &right,
		const VectorImage &leftMask, const VectorImage &rightMask, 
		int x1, int y1, int x2, int y2);

	//! .
	double weightedMedian(const DepthMap &depths, const VectorImage &mask, int x, int y);
	
	//! .
	RGBA colorFromDepth(double depth) const;
	
private:
	Camera leftView, rightView;

	VectorImage left, leftMask, resultLeft;
	VectorImage right, rightMask, resultRight;
	
	DepthMap computedDepthLeft;
	DepthMap computedDepthRight;
	
	double minDepth, maxDepth;
	int numDepthLevels;
	double imageScale;

	
};

#endif // STEREO_HPP
