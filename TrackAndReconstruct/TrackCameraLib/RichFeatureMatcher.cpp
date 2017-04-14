#include "RichFeatureMatcher.h"

#include "FindCameraMatrices.h"

#include <iostream>
#include <set>

using namespace std;
using namespace cv;
//调用OpenCV进行特征点检测
RichFeatureMatcher::RichFeatureMatcher(std::vector<cv::Mat>& imgs_, std::vector < std::vector<cv::KeyPoint> >& imgpts_)
	:imgpts(imgpts_), imgs(imgs_)
{
	detector = FeatureDetector::create("PyramidFAST");
	extractor = DescriptorExtractor::create("ORB");
	std::cout << "******************* extract feature points ********************" << std::endl;
	detector->detect(imgs, imgpts);
	extractor->compute(imgs, imgpts, descriptors);
	std::cout << "******************* Done ***************************" << std::endl;
}
//特征点匹配，寻找足够好的特征点存储
void RichFeatureMatcher::MatchFeatures(int idx_i, int idx_j, std::vector<cv::DMatch>* matches /* = NULL */)
{
	const vector<KeyPoint>& imgpts1 = imgpts[idx_i];
	const vector<KeyPoint>& imgpts2 = imgpts[idx_j];
	const Mat& descriptors_1 = descriptors[idx_i];
	const Mat& descriptors_2 = descriptors[idx_j];
	
	std::vector< DMatch > good_matches_, very_good_matches_;
	std::vector<KeyPoint> keypoints_1, keypoints_2;

	stringstream ss; ss << "imgpts1 has " << imgpts1.size() << " points (descriptors " << descriptors_1.rows << ")" << endl;
	cout << ss.str();
	stringstream ss1; ss1 << "imgpts2 has " << imgpts2.size() << " points (descriptors " << descriptors_2.rows << ")" << endl;
	cout << ss1.str();

	keypoints_1 = imgpts1;
	keypoints_2 = imgpts2;
	
	if (descriptors_1.empty()) {
		CV_Error(0, "descriptors_1 is empty");
	}
	if (descriptors_2.empty()) {
		CV_Error(0, "descriptors_2 is empty");
	}
	
	BFMatcher matcher(NORM_HAMMING, true);
	std::vector< DMatch > matches_;
	if (matches == NULL) {
		matches = &matches_;
	}
	if (matches->size() == 0) {
		matcher.match(descriptors_1, descriptors_2, *matches);
	}
	
	assert(matches->size() > 0);
	
	vector<KeyPoint> imgpts1_good, imgpts2_good;

	std::set<int> existing_trainIdx;
	for (unsigned int i = 0; i < matches->size(); i++)	{
		if ((*matches)[i].trainIdx <= 0) {
			(*matches)[i].trainIdx = (*matches)[i].imgIdx;
		}

		if (existing_trainIdx.find((*matches)[i].trainIdx) == existing_trainIdx.end() &&
			(*matches)[i].trainIdx >= 0 && (*matches)[i].trainIdx < (int)(keypoints_2.size()) ) {
				good_matches_.push_back((*matches)[i]);
				imgpts1_good.push_back(keypoints_1[(*matches)[i].queryIdx]);
				imgpts2_good.push_back(keypoints_2[(*matches)[i].trainIdx]);
				existing_trainIdx.insert((*matches)[i].trainIdx);
			}
	}

	vector<uchar> status;
	vector<KeyPoint> imgpts2_very_good, imgpts1_very_good;

	assert(imgpts1_good.size() > 0);
	assert(imgpts2_good.size() > 0);
	assert(good_matches_.size() > 0);
	assert(imgpts1_good.size() == imgpts2_good.size() && imgpts1_good.size() == good_matches_.size());

	GetFundamentalMat(keypoints_1, keypoints_2, imgpts1_very_good, imgpts2_very_good, good_matches_);
}