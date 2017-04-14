#ifndef BUNDLEADJUSTER_H
#define BUNDLEADJUSTER_H

#include <vector>
#include <map>
#include <opencv2/core/core.hpp>
#include "data.h"

class BundleAdjuster {
public:
	void adjustBundle(std::vector<CloudPoint>& pointcloud, 
					  cv::Mat& cam_matrix,
					  const std::vector<std::vector<cv::KeyPoint> >& imgpts,
					  std::map<int ,cv::Matx34d>& Pmats);
private:
	int Count2DMeasurements(const std::vector<CloudPoint>& pointcloud);
};

#endif