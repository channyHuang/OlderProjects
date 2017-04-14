#include "MultiCameraPnP.h"
#include "BundleAdjuster.h"

#include <fstream>
#include <vector>
#include <iostream>

#include <opencv2/gpu/gpu.hpp>
//#include "opencv2/core/cuda.hpp"
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
//using namespace cv;

bool sort_by_first(pair<int, pair<int, int> > a, pair<int, pair<int, int> > b) { return a.first < b.first; }
//返回两图像的单应性矩阵H
int MultiCameraPnP::FindHomographyInliers2Views(int vi, int vj)
{
	vector<cv::KeyPoint> ikpts, jkpts; vector<cv::Point2f> ipts, jpts;
	GetAlignedPointsFromMatch(imgpts[vi], imgpts[vj], matches_matrix[make_pair(vi, vj)], ikpts, jkpts);
	KeyPointsToPoints(ikpts, ipts); 
	KeyPointsToPoints(jkpts, jpts);

	double minVal, maxVal; cv::minMaxIdx(ipts, &minVal, &maxVal); 

	vector<uchar> status;
	cv::Mat H = cv::findHomography(ipts, jpts, status, CV_RANSAC, 0.004 * maxVal); //threshold from Snavely07
	return cv::countNonZero(status); 
}

void MultiCameraPnP::RecoverDepthFromImages()
{
	if (!features_matched) 
		OnlyMatchFeatures();
	std::cout << "*********************** Depth Recovery Start ****************************" << std::endl;
	
	PruneMatchesBasedOnF();
	GetBaseLineTriangulation();
	AdjustCurrentBundle();

	cv::Matx34d P1 = Pmats[m_second_view];
	cv::Mat_<double> t = (cv::Mat_<double>(1, 3) << P1(0, 3), P1(1, 3), P1(2, 3));
	cv::Mat_<double> R = (cv::Mat_<double>(3, 3) << P1(0, 0), P1(0, 1), P1(0, 2),
		P1(1, 0), P1(1, 1), P1(1, 2),
		P1(2, 0), P1(2, 1), P1(2, 2));
	cv::Mat_<double> rvec(1, 3); 
	Rodrigues(R, rvec);

// 	m_first_view = 0;
// 	m_second_view = 1;
// 
// 	done_views.insert(m_first_view);
// 	done_views.insert(m_second_view);
// 	good_views.insert(m_first_view);
// 	good_views.insert(m_second_view);

	string visionparamName = DATA_PATH + string("visionparam.txt");
	ofstream fs(visionparamName);

	while (done_views.size() !=imgs.size()) {
		unsigned int max_2d3d_view = -1, max_2d3d_count = 0;
		vector<cv::Point3f> max_3d; 
		vector<cv::Point2f> max_2d;
		//for (unsigned int _i = 0; _i < imgs.size(); _i++) {
		set<int>::iterator current_view = done_views.end();
		for (unsigned int _i = max(0, *current_view - 3); _i < imgs.size() && _i < *current_view + 3; _i++) {
			if (done_views.find(_i) != done_views.end()) continue;

			vector<cv::Point3f> tmp3d;
			vector<cv::Point2f> tmp2d;
			cout << "Frame " << _i << ": ";
			Find2D3DCorrespondences(_i, tmp3d, tmp2d);
			if (tmp3d.size() >= max_2d3d_count) {
				max_2d3d_count = tmp3d.size();
				max_2d3d_view = _i;
				max_3d = tmp3d; max_2d = tmp2d;
			}
		}
		int i = max_2d3d_view;
		
		char str[100];
		sprintf(str, "%02d_points.txt", done_views.size());
		string outputpt = DATA_PATH + string(str) + string("points.txt");
		ofstream outputfs(outputpt);
		for (int ptidx = 0; ptidx < max_3d.size(); ptidx++) {
			outputfs << max_3d.at(ptidx).x << " " << max_3d.at(ptidx).y << " " << max_3d.at(ptidx).z << endl;
		}
		outputfs.close();

		std::cout << "-------------------------- Frame " << i << " --------------------------\n";
		done_views.insert(i);

		bool pose_estimated = FindPoseEstimation(i, rvec, t, R, max_3d, max_2d);
		if (!pose_estimated)
			continue;

		fs << "Frame " << i << "\n";
		fs << R(0, 0) << " " << R(0, 1) << " " << R(0, 2) << endl;
		fs << R(1, 0) << " " << R(1, 1) << " " << R(1, 2) << endl;
		fs << R(2, 0) << " " << R(2, 1) << " " << R(2, 2) << endl;
		fs << t(0) << " " << t(1) << " " << t(2) << endl;

		Pmats[i] = cv::Matx34d(R(0, 0), R(0, 1), R(0, 2), t(0),
			R(1, 0), R(1, 1), R(1, 2), t(1),
			R(2, 0), R(2, 1), R(2, 2), t(2));

		for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view)
		{
			int view = *done_view;
			if (view <= i || view > i+3) continue;

			cout << " -> frame" << view << endl;

			vector<CloudPoint> new_triangulated;
			vector<int> add_to_cloud;
			bool good_triangulation = TriangulatePointsBetweenViews(i, view, new_triangulated, add_to_cloud);
			if (!good_triangulation) continue;

			std::cout << "before triangulation: " << pcloud.size();
			for (int j = 0; j < add_to_cloud.size(); j++) {
				if (add_to_cloud[j] == 1)
					pcloud.push_back(new_triangulated[j]);
			}
			std::cout << " after " << pcloud.size() << std::endl;

		}
		good_views.insert(i);

		AdjustCurrentBundle();
	}

	fs.close();

	std::cout << "************** Depth Recovery Done *************" << std::endl;
}
//特征点匹配
void MultiCameraPnP::PruneMatchesBasedOnF()
{
	for (int _i = 0; _i < imgs.size() - 1; _i++) {
		for (unsigned int _j = _i + 1; _j < imgs.size() && _j < _i + 4; _j++) {
			int older_view = _i, working_view = _j;

			GetFundamentalMat(imgpts[older_view],
				imgpts[working_view],
				imgpts_good[older_view],
				imgpts_good[working_view],
				matches_matrix[std::make_pair(older_view, working_view)]);
			matches_matrix[std::make_pair(working_view, older_view)] = FlipMatches(matches_matrix[std::make_pair(older_view, working_view)]);
		}
	}
}
//从图像点恢复三维点
void MultiCameraPnP::Find2D3DCorrespondences(int working_view,
	std::vector<cv::Point3f>& ppcloud,
	std::vector<cv::Point2f>& imgPoints)
{
	ppcloud.clear();
	imgPoints.clear();

	vector<int> pcloud_status(pcloud.size(), 0);
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view)	{
	for (int old_view = 0; old_view < image.size(); old_view++ ) {
		if (old_view < working_view - 3 || old_view > working_view + 3 || old_view == working_view) continue; 
		std::vector<cv::DMatch> matches_from_old_to_working = matches_matrix[std::make_pair(old_view, working_view)];

		for (unsigned int match_from_old_view = 0; match_from_old_view < matches_from_old_to_working.size(); match_from_old_view++) {
			int idx_in_old_view = matches_from_old_to_working[match_from_old_view].queryIdx;

			for (unsigned int pcldp = 0; pcldp < pcloud.size(); pcldp++) {
				if (idx_in_old_view == pcloud[pcldp].imgpt_for_img[old_view] && pcloud_status[pcldp] == 0) {
					ppcloud.push_back(pcloud[pcldp].pt);
					imgPoints.push_back(imgpts[working_view][matches_from_old_to_working[match_from_old_view].trainIdx].pt);
					pcloud_status[pcldp] = 1;
					break;
				}
			}
		}
	}
	cout << "found " << ppcloud.size() << " 3d-2d point correspondences" << endl;
}
//从两幅图像中初始化三维点云
void MultiCameraPnP::GetBaseLineTriangulation()
{
	cv::Matx34d P(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0),
		P1(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0);

	std::vector<CloudPoint> tmp_pcloud;

	cout << "Find highest match...";
	list<pair<int, pair<int, int> > > matches_sizes;

	for (std::map<std::pair<int, int>, std::vector<cv::DMatch> >::iterator i = matches_matrix.begin(); i != matches_matrix.end(); ++i) {
		if ((*i).second.size() < 100)
			matches_sizes.push_back(make_pair(100, (*i).first));
		else {
			int Hinliers = FindHomographyInliers2Views((*i).first.first, (*i).first.second);
			int percent = (int)(((double)Hinliers) / ((double)(*i).second.size()) * 100.0);
			cout << "[" << (*i).first.first << "," << (*i).first.second << " = " << percent << "] ";
			matches_sizes.push_back(make_pair((int)percent, (*i).first));
		}
	}
	cout << endl;
	matches_sizes.sort(sort_by_first);

	bool goodF = false;
	int highest_pair = 0;
	m_first_view = m_second_view = 0;

	for (list<pair<int, pair<int, int> > >::iterator highest_pair = matches_sizes.begin();
		highest_pair != matches_sizes.end() && !goodF;
		++highest_pair) {
		m_second_view = (*highest_pair).second.second;
		m_first_view = (*highest_pair).second.first;

		std::cout << " -------- Frame " << m_first_view << " and Frame " << m_second_view << " -------- " << std::endl;

		goodF = FindCameraMatrices(K, Kinv, distortion_coeff,
			imgpts[m_first_view],
			imgpts[m_second_view],
			imgpts_good[m_first_view],
			imgpts_good[m_second_view],
			P,
			P1,
			matches_matrix[std::make_pair(m_first_view, m_second_view)], tmp_pcloud);

		if (goodF) {
			vector<CloudPoint> new_triangulated;
			vector<int> add_to_cloud;

			Pmats[m_first_view] = P;
			Pmats[m_second_view] = P1;

			bool good_triangulation = TriangulatePointsBetweenViews(m_second_view, m_first_view, new_triangulated, add_to_cloud);
			if (!good_triangulation || cv::countNonZero(add_to_cloud) < 10) {
				std::cout << "triangulation failed" << std::endl;
				goodF = false;
				Pmats[m_first_view] = 0;
				Pmats[m_second_view] = 0;
				m_second_view++;
			}
			else {
				std::cout << "before triangulation: " << pcloud.size();
				for (unsigned int j = 0; j < add_to_cloud.size(); j++) {
					if (add_to_cloud[j] == 1)
						pcloud.push_back(new_triangulated[j]);
				}
				std::cout << " after " << pcloud.size() << std::endl;
			}
		}
	}
	if (!goodF) {
		cerr << "Cannot find a good pair of images to obtain a baseline triangulation" << endl;
		exit(0);
	}

	cout << "Taking baseline from frame " << m_first_view << " and frame " << m_second_view << endl;

}

bool MultiCameraPnP::TriangulatePointsBetweenViews(
	int working_view,
	int older_view,
	std::vector<struct CloudPoint>& new_triangulated,
	std::vector<int>& add_to_cloud
	)
{
	cout << " Triangulate frame " << working_view << " and frame " << older_view << endl;

	cv::Matx34d P = Pmats[older_view];
	cv::Matx34d P1 = Pmats[working_view];

	std::vector<cv::KeyPoint> pt_set1, pt_set2;
	std::vector<cv::DMatch> matches = matches_matrix[std::make_pair(older_view, working_view)];
	GetAlignedPointsFromMatch(imgpts[older_view], imgpts[working_view], matches, pt_set1, pt_set2);

	double reproj_error = TriangulatePoints(pt_set1, pt_set2, K, Kinv, distortion_coeff, P, P1, new_triangulated, correspImg1Pt);
	std::cout << "triangulation reproj error " << reproj_error << std::endl;

	vector<uchar> trig_status;
	if (!TestTriangulation(new_triangulated, P, trig_status) || !TestTriangulation(new_triangulated, P1, trig_status)) {
		cerr << "Triangulation did not succeed" << endl;
		return false;
	}

	vector<double> reprj_errors;
	for (int i = 0; i < new_triangulated.size(); i++) { reprj_errors.push_back(new_triangulated[i].reprojection_error); }
	std::sort(reprj_errors.begin(), reprj_errors.end());
	double reprj_err_cutoff = reprj_errors[4 * reprj_errors.size() / 5] * 2.4;

	vector<CloudPoint> new_triangulated_filtered;
	std::vector<cv::DMatch> new_matches;
	for (int i = 0; i < new_triangulated.size(); i++) {
		if (trig_status[i] == 0)
			continue;
		if (new_triangulated[i].reprojection_error > 16.0)
			continue;
		if (new_triangulated[i].reprojection_error < 4.0 ||
			new_triangulated[i].reprojection_error < reprj_err_cutoff)	{
			new_triangulated_filtered.push_back(new_triangulated[i]);
			new_matches.push_back(matches[i]);
		}
		else continue;
	}

	cout << "filtered out " << (new_triangulated.size() - new_triangulated_filtered.size()) << " high-error points" << endl;

	if (new_triangulated_filtered.size() <= 0) return false;

	new_triangulated = new_triangulated_filtered;

	matches = new_matches;
	matches_matrix[std::make_pair(older_view, working_view)] = new_matches; 
	matches_matrix[std::make_pair(working_view, older_view)] = FlipMatches(new_matches);
	add_to_cloud.clear();
	add_to_cloud.resize(new_triangulated.size(), 1);
	int found_other_views_count = 0;
	int num_views = imgs.size();

	for (int j = 0; j < new_triangulated.size(); j++) {
		new_triangulated[j].imgpt_for_img = std::vector<int>(imgs.size(), -1);
		new_triangulated[j].imgpt_for_img[older_view] = matches[j].queryIdx;
		new_triangulated[j].imgpt_for_img[working_view] = matches[j].trainIdx;
		bool found_in_other_view = false;
		for (unsigned int view_ = 0; view_ < num_views; view_++) {
			if (view_ != older_view) {
				std::vector<cv::DMatch> submatches = matches_matrix[std::make_pair(view_, working_view)];
				for (unsigned int ii = 0; ii < submatches.size(); ii++) {
					if (submatches[ii].trainIdx == matches[j].trainIdx &&
						!found_in_other_view)	{
						for (unsigned int pt3d = 0; pt3d < pcloud.size(); pt3d++) {
							if (pcloud[pt3d].imgpt_for_img[view_] == submatches[ii].queryIdx)	{
								pcloud[pt3d].imgpt_for_img[working_view] = matches[j].trainIdx;
								pcloud[pt3d].imgpt_for_img[older_view] = matches[j].queryIdx;
								found_in_other_view = true;
								add_to_cloud[j] = 0;
							}
						}
					}
				}
			}
		}
		{
			if (found_in_other_view) {
				found_other_views_count++;
			}
			else {
				add_to_cloud[j] = 1;
			}
		}
	}
	std::cout << found_other_views_count << "/" << new_triangulated.size() << " points were found in other views, adding " << cv::countNonZero(add_to_cloud) << " new\n";
	return true;
}

	bool MultiCameraPnP::FindPoseEstimation(
		int working_view,
		cv::Mat_<double>& rvec,
		cv::Mat_<double>& t,
		cv::Mat_<double>& R,
		std::vector<cv::Point3f> ppcloud,
		std::vector<cv::Point2f> imgPoints
		)
	{
		if (ppcloud.size() <= 7 || imgPoints.size() <= 7 || ppcloud.size() != imgPoints.size()) {
			cerr << "Con't find [enough] corresponding cloud points..." << endl;
			return false;
		}
		
		vector<int> inliers;
		if (!use_gpu) {
			double minVal, maxVal;
			cv::minMaxIdx(imgPoints, &minVal, &maxVal);
			CV_PROFILE("solvePnPRansac", cv::solvePnPRansac(ppcloud, imgPoints, K, distortion_coeff, rvec, t, true, 1000, 0.006 * maxVal, 0.25 * (double)(imgPoints.size()), inliers, CV_EPNP);)
		}
		else {
			cv::Mat ppcloud_m(ppcloud);
			ppcloud_m = ppcloud_m.t();
			cv::Mat imgPoints_m(imgPoints);
			imgPoints_m = imgPoints_m.t();
			cv::Mat rvec_, t_;

			cv::gpu::solvePnPRansac(ppcloud_m, imgPoints_m, K_32f, distortion_coeff, rvec_, t_, false);

			rvec_.convertTo(rvec, CV_64FC1);
			t_.convertTo(t, CV_64FC1);
		}

		vector<cv::Point2f> projected3D;
		cv::projectPoints(ppcloud, rvec, t, K, distortion_coeff, projected3D);

		if (inliers.size() == 0) {
			for (int i = 0; i < projected3D.size(); i++) {
				if (norm(projected3D[i] - imgPoints[i]) < 10.0)
					inliers.push_back(i);
			}
		}

		if (inliers.size() < (double)(imgPoints.size()) / 5.0) {
			cerr << "not enough inliers to consider a good pose (" << inliers.size() << "/" << imgPoints.size() << ")" << endl;
			return false;
		}

		if (cv::norm(t) > 200.0) {
			cerr << "estimated camera movement is too big, skip this camera\r\n";
			return false;
		}

		cv::Rodrigues(rvec, R);
		if (!CheckCoherentRotation(R)) {
			cerr << "rotation is incoherent. we should try a different base view..." << endl;
			return false;
		}

		std::cout << "found t = " << t << "\nR = \n" << R << std::endl;
		return true;
	}

	void MultiCameraPnP::AdjustCurrentBundle()
	{
		cout << "------------------- Bundle Adjustment -------------------" << endl;

		cv::Mat _cam_matrix = K;
		BundleAdjuster BA;
		BA.adjustBundle(pcloud, _cam_matrix, imgpts, Pmats);
		K = cam_matrix;
		Kinv = K.inv();

		cout << "use new K " << endl << K << endl;

		GetRGBForPointCloud(pcloud, pointCloudRGB);
	}