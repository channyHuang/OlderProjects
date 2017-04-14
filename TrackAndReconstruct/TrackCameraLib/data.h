#ifndef DATA_H
#define DATA_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <iostream>
#include <list>
#include <set>

using namespace std;
//using namespace cv;

#define CV_PROFILE(msg,code) code

struct CloudPoint {
	cv::Point3d pt;
	std::vector<int> imgpt_for_img;
	double reprojection_error;
};

extern string videoName;
extern int skipframe;

extern vector<cv::Mat> image;

extern vector<cv::Mat> keyImage;

extern char* DATA_PATH;

std::vector<cv::DMatch> FlipMatches(const std::vector<cv::DMatch>& matches);
void KeyPointsToPoints(const std::vector<cv::KeyPoint>& kps, std::vector<cv::Point2f>& ps);
void PointsToKeyPoints(const std::vector<cv::Point2f>& ps, std::vector<cv::KeyPoint>& kps);
std::vector<cv::Point3d> CloudPointsToPoints(const std::vector<CloudPoint> cpts);
void GetAlignedPointsFromMatch(const std::vector<cv::KeyPoint>& imgpts1,
	const std::vector<cv::KeyPoint>& imgpts2,
	const std::vector<cv::DMatch>& matches,
	std::vector<cv::KeyPoint>& pt_set1,
	std::vector<cv::KeyPoint>& pt_set2);

class IFeatureMatcher {
public:
	virtual void MatchFeatures(int idx_i, int idx_j, std::vector<cv::DMatch>* matches) = 0;
};

#endif