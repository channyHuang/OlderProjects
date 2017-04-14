#ifndef RICHFEATUREMATCHER_H
#define RICHFEATUREMATCHER_H

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "data.h"

class RichFeatureMatcher : public IFeatureMatcher {
private:
	cv::Ptr<cv::FeatureDetector> detector;
	cv::Ptr<cv::DescriptorExtractor> extractor;

	std::vector<cv::Mat> descriptors;

	std::vector<cv::Mat>& imgs;
	std::vector<std::vector<cv::KeyPoint> >& imgpts;
public:

	RichFeatureMatcher(std::vector<cv::Mat>& imgs, std::vector<std::vector<cv::KeyPoint> >& imgpts);
	void MatchFeatures(int idx_i, int idx_j, std::vector<cv::DMatch>* matches = NULL);
};

#endif