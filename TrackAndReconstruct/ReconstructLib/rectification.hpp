#ifndef RECTIFICATION_H
#define RECTIFICATION_H
#include <tuple>
#include <opencv2/opencv.hpp>

#include "camera.hpp"
#include <QtGui/QImage>

void paramTransRectification(Camera leftview, Camera rightview, QImage left, QImage right);

void get_rectification_maxtrix(
	cv::Mat K, cv::Mat dist,
	cv::Mat R1, cv::Mat R2,//rotation matrix
	cv::Mat T1, cv::Mat T2,//translation vector
	cv::Size image_size,
	cv::Mat& H1, 
	cv::Mat& H2,
	cv::Mat& P1, 
	cv::Mat& P2,
	cv::Mat& Q
	);

#endif