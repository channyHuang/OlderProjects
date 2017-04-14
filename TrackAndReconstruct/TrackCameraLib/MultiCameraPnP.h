#ifndef MULTICAMERAPNP_H
#define MULTICAMERAPNP_H

#include "MultiCameraDistance.h"

class MultiCameraPnP : public MultiCameraDistance
{
	std::vector<CloudPoint> pointcloud_beforeBA;
	std::vector<cv::Vec3b> pointCloudRGB_beforeBA;

public:
	MultiCameraPnP(const std::vector<cv::Mat>& imgs_)
		: MultiCameraDistance(imgs_)
	{

	}

	std::vector<cv::Point3d> getPointCloudBeforeBA() { return CloudPointsToPoints(pointcloud_beforeBA); }
	const std::vector<cv::Vec3b>& getPointCloudRGBBeforeBA() { return pointCloudRGB_beforeBA; }

	virtual void RecoverDepthFromImages();
private:
	void PruneMatchesBasedOnF();
	void AdjustCurrentBundle();
	void GetBaseLineTriangulation();
	void Find2D3DCorrespondences(int working_view,
		std::vector<cv::Point3f>& ppcloud,
		std::vector<cv::Point2f>& imgPoints);

	bool TriangulatePointsBetweenViews(
		int working_view,
		int second_view,
		std::vector<struct CloudPoint>& new_triangulated,
		std::vector<int>& add_to_cloud
		);

	int FindHomographyInliers2Views(int vi, int vj);
	int m_first_view, m_second_view;
	std::set<int> done_views, good_views;

	bool MultiCameraPnP::FindPoseEstimation(
		int working_view,
		cv::Mat_<double>& rvec,
		cv::Mat_<double>& t,
		cv::Mat_<double>& R,
		std::vector<cv::Point3f> ppcloud,
		std::vector<cv::Point2f> imgPoints
		);

};

#endif