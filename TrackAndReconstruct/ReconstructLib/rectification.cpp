#include "rectification.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace std;
using namespace cv;

void paramTransRectification(Camera leftview, Camera rightview, QImage left, QImage right) {
/*	Mat K = Mat::eye(3, 3, CV_64F);
	Mat dist = Mat::zeros(4, 1, CV_64F);
	Mat R1 = Mat::eye(3, 3, CV_64F), R2 = Mat::eye(3, 3, CV_64F), T1 = Mat::zeros(3, 1, CV_64F), T2 = Mat::zeros(3, 1, CV_64F);
	Size image_size(left.width(), left.height());
	Mat H1, H2, P1, P2, Q;

	for (int i = 0; i < 3; i++) {
		T1.at<double>(i) = leftview.t()(i);
		T2.at<double>(i) = rightview.t()(i);
		for (int j = 0; j < 3; j++) {
			K.at<double>(i, j) = leftview.K()(i, j);
			R1.at<double>(i, j) = leftview.R()(i, j);
			R2.at<double>(i, j) = rightview.R()(i, j);
		}
	}
*/
	string TEST_DATA_DIR = "F:/mycode/stereoTest/stereoTest/test/data/";
	string rectification_dir = string(TEST_DATA_DIR) + "rectification/";
	FileStorage fs(rectification_dir + "camera_params.yaml", FileStorage::READ);
	Mat image0 = imread(rectification_dir + "pic_0.jpg");
	Mat image1 = imread(rectification_dir + "pic_3.jpg");
	Mat camera_matrix, dist;
	fs["camera_matrix"] >> camera_matrix;
	fs["distortion_coefficients"] >> dist;
	vector<Mat> rotations;
	fs["rotation"] >> rotations;
	vector<Mat> translations;
	fs["transformation"] >> translations;
	cv::Mat H1, H2, P1, P2, Q;
	get_rectification_maxtrix(camera_matrix,
		dist,
		rotations[0],
		rotations[1],
		translations[0],
		translations[1],
		image0.size(),
		H1,
		H2,
		P1,
		P2,
		Q
		);

//	get_rectification_maxtrix(K, dist, R1, R2, T1, T2, image_size, H1, H2, P1, P2, Q);

	Mat rmap[2][2];
	//Precompute maps for cv::remap()
/*	initUndistortRectifyMap(K, dist, R1, P1, image_size, CV_16SC2, rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(K, dist, R2, P2, image_size, CV_16SC2, rmap[1][0], rmap[1][1]);
	*/
	initUndistortRectifyMap(camera_matrix, dist, rotations[0], P1, image0.size(), CV_16SC2, rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(camera_matrix, dist, rotations[1], P2, image0.size(), CV_16SC2, rmap[1][0], rmap[1][1]);

	Size image_size = image0.size();
	Mat canvas;
	double sf;
	int w, h;
	sf = 600. / (image_size.width > image_size.height ? image_size.width : image_size.height);
	w = cvRound(image_size.width*sf);
	h = cvRound(image_size.height*sf);
	canvas.create(h, w * 2, CV_8UC3);
	Mat img[2], rimg, cimg;
//	img[0] = Mat(left.height(), left.width(), CV_8UC4, (uchar*)left.bits(), left.bytesPerLine());
//	img[1] = Mat(right.height(), right.width(), CV_8UC4, (uchar*)right.bits(), right.bytesPerLine());
	img[0] = image0;
	img[1] = image1;
	for (int k = 0; k < 2; k++) {
		remap(img[k], rimg, rmap[k][0], rmap[k][1], CV_INTER_LINEAR);
		//cvtColor(rimg, cimg, COLOR_GRAY2BGR);
		cimg = rimg.clone();
		waitKey(0);
		Mat canvasPart = canvas(Rect(w*k, 0, w, h));
		resize(cimg, canvasPart, canvasPart.size(), 0, 0, CV_INTER_AREA);
	}
	
	for (int j = 0; j < canvas.rows; j += 16)
		line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
	imshow("rectified", canvas);
	waitKey(0);
}

void get_rectification_maxtrix(
	cv::Mat K, cv::Mat dist,
	cv::Mat R1, cv::Mat R2,
	cv::Mat T1, cv::Mat T2,
	cv::Size image_size,
	cv::Mat& H1, cv::Mat& H2,
	cv::Mat& P1, cv::Mat& P2,
	cv::Mat& Q
	) {
	auto R = R2*R1.inv();
	auto T = T2 - R*T1;

	/*http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html*/
	stereoRectify(K, dist,
		K, dist,
		image_size,
		R,
		T,
		H1,
		H2,
		P1,
		P2,
		Q
	);
	//H1 = K*H1*K.inv();
	//H2 = K*H2*K.inv();
}