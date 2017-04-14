#include "FindCameraMatrices.h"
#include "Triangulation.h"

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>

#include <iostream>

using namespace cv;
using namespace std;

#ifndef CV_PCA_DATA_AS_ROW
#define CV_PCA_DATA_AS_ROW 0
#endif

//对本质矩阵E进行SVD分解，用于函数DecomposeEtoRandT()
void TakeSVDOfE(Mat_<double>& E, Mat& svd_u, Mat& svd_vt, Mat& svd_w)
{
	SVD svd(E, SVD::MODIFY_A);
	svd_u = svd.u;
	svd_vt = svd.vt;
	svd_w = svd.w;

	cout << "----------------- SVD -------------------" << endl;
	cout << "U:\n" << svd_u << "\nW:\n" << svd_w << "\nVt:\n" << svd_vt << endl;
	cout << "-----------------------------------------" << endl;
}
//由本质矩阵E计算摄像机外参R和t,用于函数FindCameraMatrices()
bool DecomposeEtoRandT(Mat_<double>& E, Mat_<double>& R1, Mat_<double>& R2, Mat_<double>& t1, Mat_<double>& t2)
{
	Mat svd_u, svd_vt, svd_w;
	TakeSVDOfE(E, svd_u, svd_vt, svd_w);

	double singular_values_ratio = fabsf(svd_w.at<double>(0) / svd_w.at<double>(1));
	if (singular_values_ratio > 1.0) singular_values_ratio = 1.0 / singular_values_ratio;
	if (singular_values_ratio < 0.7) {
		cout << "singular values are too far apart\n";
		//return false;
	}

	Matx33d W(0, -1, 0, 1, 0, 0, 0, 0, 1);
	Matx33d Wt(0, 1, 0, -1, 0, 0, 0, 0, 1);
	R1 = svd_u * Mat(W) * svd_vt;
	R2 = svd_u * Mat(Wt) * svd_vt;
	t1 = svd_u.col(2);
	t2 = -svd_u.col(2);

	return true;
}
//基本矩阵F
Mat GetFundamentalMat(const vector<KeyPoint>& imgpts1,
	const vector<KeyPoint>& imgpts2,
	vector<KeyPoint>& imgpts1_good,
	vector<KeyPoint>& imgpts2_good,
	vector<DMatch>& matches)
{
	vector<uchar> status(imgpts1.size());

	imgpts1_good.clear(); 
	imgpts2_good.clear();

	vector<KeyPoint> imgpts1_tmp;
	vector<KeyPoint> imgpts2_tmp;
	if (matches.size() <= 0) {
		imgpts1_tmp = imgpts1;
		imgpts2_tmp = imgpts2;
	}
	else {
		GetAlignedPointsFromMatch(imgpts1, imgpts2, matches, imgpts1_tmp, imgpts2_tmp);
	}

	Mat F;
	{
		vector<Point2f> pts1, pts2;
		KeyPointsToPoints(imgpts1_tmp, pts1);
		KeyPointsToPoints(imgpts2_tmp, pts2);

		double minVal, maxVal;
		cv::minMaxIdx(pts1, &minVal, &maxVal);
		F = findFundamentalMat(pts1, pts2, FM_RANSAC, 0.006*maxVal, 0.99, status);
	}

	vector<DMatch> new_matches;
	cout << "F keeping " << countNonZero(status) << " / " << status.size() << endl;

	for (unsigned int i = 0; i < status.size(); i++) {
		if (status[i]) {
			imgpts1_good.push_back(imgpts1_tmp[i]);
			imgpts2_good.push_back(imgpts2_tmp[i]);

			if (matches.size() <= 0) {
				new_matches.push_back(DMatch(matches[i].queryIdx, matches[i].trainIdx, matches[i].distance));
			}
			else {
				new_matches.push_back(matches[i]);
			}
		}
	}

	cout << matches.size() << " matches before, " << new_matches.size() << " new matches after Fundamental Matrix\n";
	matches = new_matches;

	return F;
}
//计算投影矩阵P
bool FindCameraMatrices(const Mat& K,
	const Mat& Kinv,
	const Mat& distcoeff,
	const vector<KeyPoint>& imgpts1,
	const vector<KeyPoint>& imgpts2,
	vector<KeyPoint>& imgpts1_good,
	vector<KeyPoint>& imgpts2_good,
	Matx34d& P,
	Matx34d& P1,
	vector<DMatch>& matches,
	vector<CloudPoint>& outCloud)
{
	{
		cout << "****************** Find Camera Matrices ********************" << endl;
		double t = getTickCount();

		Mat F = GetFundamentalMat(imgpts1, imgpts2, imgpts1_good, imgpts2_good, matches);
		if (matches.size() < 100) {
			cerr << "Not enough inliers after F matrix" << endl;
			//return false;
		}

		Mat_<double> E = K.t() * F * K;

		if (fabsf(determinant(E)) > 1e-07) {
			cout << "det(E) != 0 : " << determinant(E) << "\n";
			P1 = 0;
			//return false;
		}

		Mat_<double> R1(3, 3);
		Mat_<double> R2(3, 3);
		Mat_<double> t1(1, 3);
		Mat_<double> t2(1, 3);

		{
			if (!DecomposeEtoRandT(E, R1, R2, t1, t2)) return false;
			if (determinant(R1) + 1.0 < 1e-9) {
				cout << "det(R) == -1 [" << determinant(R1) << "]: flip E's sign" << endl;
				E = -E;
				DecomposeEtoRandT(E, R1, R2, t1, t2);
			}
			if (!CheckCoherentRotation(R1)) {
				cout << "resulting rotation is not coherent" << endl;
				P1 = 0;
				//return false;
			}

			P1 = Matx34d(R1(0, 0), R1(0, 1), R1(0, 2), t1(0),
				R1(1, 0), R1(1, 1), R1(1, 2), t1(1),
				R1(2, 0), R1(2, 1), R1(2, 2), t1(2));
			cout << "Testing P1 " << endl << Mat(P1) << endl;

			vector<CloudPoint> pcloud, pcloud1; vector<KeyPoint> corresp;
			double reproj_error1 = TriangulatePoints(imgpts1_good, imgpts2_good, K, Kinv, distcoeff, P, P1, pcloud, corresp);
			double reproj_error2 = TriangulatePoints(imgpts2_good, imgpts1_good, K, Kinv, distcoeff, P1, P, pcloud1, corresp);
			vector<uchar> tmp_status;

			if (!TestTriangulation(pcloud, P1, tmp_status) || !TestTriangulation(pcloud1, P, tmp_status) || reproj_error1 > 100.0 || reproj_error2 > 100.0) {
				P1 = Matx34d(R1(0, 0), R1(0, 1), R1(0, 2), t2(0),
					R1(1, 0), R1(1, 1), R1(1, 2), t2(1),
					R1(2, 0), R1(2, 1), R1(2, 2), t2(2));
				cout << "Testing P1 " << endl << Mat(P1) << endl;

				pcloud.clear(); 
				pcloud1.clear(); 
				corresp.clear();
				reproj_error1 = TriangulatePoints(imgpts1_good, imgpts2_good, K, Kinv, distcoeff, P, P1, pcloud, corresp);
				reproj_error2 = TriangulatePoints(imgpts2_good, imgpts1_good, K, Kinv, distcoeff, P1, P, pcloud1, corresp);

				if (!TestTriangulation(pcloud, P1, tmp_status) || !TestTriangulation(pcloud1, P, tmp_status) || reproj_error1 > 100.0 || reproj_error2 > 100.0) {
					if (!CheckCoherentRotation(R2)) {
						cout << "resulting rotation is not coherent\n";
						P1 = 0;
						//return false;
					}

					P1 = Matx34d(R2(0, 0), R2(0, 1), R2(0, 2), t1(0),
						R2(1, 0), R2(1, 1), R2(1, 2), t1(1),
						R2(2, 0), R2(2, 1), R2(2, 2), t1(2));
					cout << "Testing P1 " << endl << Mat(P1) << endl;

					pcloud.clear(); pcloud1.clear(); corresp.clear();
					reproj_error1 = TriangulatePoints(imgpts1_good, imgpts2_good, K, Kinv, distcoeff, P, P1, pcloud, corresp);
					reproj_error2 = TriangulatePoints(imgpts2_good, imgpts1_good, K, Kinv, distcoeff, P1, P, pcloud1, corresp);

					if (!TestTriangulation(pcloud, P1, tmp_status) || !TestTriangulation(pcloud1, P, tmp_status) || reproj_error1 > 100.0 || reproj_error2 > 100.0) {
						P1 = Matx34d(R2(0, 0), R2(0, 1), R2(0, 2), t2(0),
							R2(1, 0), R2(1, 1), R2(1, 2), t2(1),
							R2(2, 0), R2(2, 1), R2(2, 2), t2(2));
						cout << "Testing P1 " << endl << Mat(P1) << endl;

						pcloud.clear(); pcloud1.clear(); corresp.clear();
						reproj_error1 = TriangulatePoints(imgpts1_good, imgpts2_good, K, Kinv, distcoeff, P, P1, pcloud, corresp);
						reproj_error2 = TriangulatePoints(imgpts2_good, imgpts1_good, K, Kinv, distcoeff, P1, P, pcloud1, corresp);

						if (!TestTriangulation(pcloud, P1, tmp_status) || !TestTriangulation(pcloud1, P, tmp_status) || reproj_error1 > 100.0 || reproj_error2 > 100.0) {
							cout << "......" << endl;
							//return false;
						}
					}
				}
			}
			for (unsigned int i = 0; i < pcloud.size(); i++) {
				outCloud.push_back(pcloud[i]);
			}
		}

		t = ((double)getTickCount() - t) / getTickFrequency();
		cout << "Done. (" << t << "s)" << endl;
	}

	return true;
}

bool TestTriangulation(const vector<CloudPoint>& pcloud, const Matx34d& P, vector<uchar>& status) {
	vector<Point3d> pcloud_pt3d = CloudPointsToPoints(pcloud);
	vector<Point3d> pcloud_pt3d_projected(pcloud_pt3d.size());

	Matx44d P4x4 = Matx44d::eye();
	for (int i = 0; i < 12; i++) P4x4.val[i] = P.val[i];

	perspectiveTransform(pcloud_pt3d, pcloud_pt3d_projected, P4x4);

	status.resize(pcloud.size(), 0);
	for (int i = 0; i<pcloud.size(); i++) {
		status[i] = (pcloud_pt3d_projected[i].z > 0) ? 1 : 0;
	}
	int count = countNonZero(status);

	double percentage = ((double)count / (double)pcloud.size());
	cout << count << "/" << pcloud.size() << " = " << percentage*100.0 << "% are in front of camera" << endl;
	if (percentage < 0.75)
		return false;

	return true;
}

bool CheckCoherentRotation(cv::Mat_<double>& R)
{
	if (fabsf(determinant(R)) - 1.0 > 1e-07) {
		cerr << "det(R) != +-1.0, this is not a rotation matrix" << endl;
		return false;
	}

	return true;
}