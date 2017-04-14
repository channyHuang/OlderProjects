#ifndef MULTIVIEWSTEREO_HPP
#define MULTIVIEWSTEREO_HPP

#include <QtGui/QImage>
#include <QtCore/QDir>

#include "ray.hpp"
#include "vectorimage.hpp"

#include "surfel.h"

QT_FORWARD_DECLARE_CLASS(Camera);

typedef std::pair<Ray3d::Point, RGBA> PLYPoint;

void outputPLYFile(const std::string &path, const std::vector<PLYPoint> &points);

void outputSurfel(const std::string &path, const std::vector<Surfel> &surfels);

class MultiViewStereo  {
public:
	void initialize(
		const std::vector<QString> &imagenames,
		const std::vector<QString> &masknames,
		const std::vector<Camera> &views,
		double minDepth, double maxDepth,
		int numDepthLevels,
		double crossCheckThreshold,
		double imageScale = 1.0);

public: 
//	int numSteps() const;
	// Return the depth map for the specified view
	QImage depthMap(Camera view) const;
	void runTask();

	//3d
	//surfel
	SurfelOperation mysurfels;

protected:
	RGBA colorFromDepth(double depth) const;
	void computeInitialEstimate(size_t viewIndex);
	void crossCheck(size_t view_index);
	double depthFromLabel(int label) const;
	bool pointFromDepth(const Ray3d &ray,
		const Ray3d::Vector &normal,
		double depth,
		Ray3d::Point &p) const;

private:

	std::vector<Camera> views;
	std::vector<std::vector<size_t> > neighbours;

	std::vector<VectorImage> images;
	std::vector<VectorImage> masks;
	std::vector<VectorImage> results;

	typedef std::vector<double> DepthMap;
	std::vector<DepthMap> computedDepths;

	double minDepth, maxDepth;
	int numDepthLevels;
	double crossCheckThreshold;
	double imageScale;

	std::vector<PLYPoint> pointcloud;
};

#endif // MULTIVIEWSTEREO_HPP