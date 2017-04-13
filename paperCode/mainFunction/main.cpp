#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <QDir>

#include "sensorFusion/fusion.h"

#include <fstream>

#include <Eigen/Eigen>

#include "depthToModel/depthToModel.h"

#include "CostVolume/CostVolume.hpp"

void testFusion() {
	//new a class of Fusion, set path, then test

	TrackSensor::Fusion fusion;

	//insure this dir contains "pic" and "calrt" two subdirs, for saving raw pictures and camera parameters.
	//And insure it contains "depthMaps" and "newCalrt" two subdirs to save depthmap and refined parameters.
	std::string path = "E:/RealFootData/data_4"; 
	fusion.setDatapath(path);
	std::string videoName = path + "/VID_20160110_192900.mp4";
	std::string xmlName = path + "/sensor_0.xml";

	fusion.test(xmlName, videoName, 100);
}

void testDepthModel() {
	std::string path = "E:/RealFootData/data_3";

	depthToModel::depthModel depthmodel;
	depthmodel.test(path);

//	cv::Mat_<double> depth = depthmodel.model2depth("points.ply", 3);
//	cv::Mat_<double> depth = depthmodel.model2depth("E:/RealFootData/data_2/sfmResult/visualSFM.nvm.cmvs/00/models/option-0000.ply", 3);
//	cv::imwrite("depth.jpg", depth);

//	depthmodel.depth2model(3);
	depthmodel.depthFusion();
}

int main(int argc, char**argv) {
//	testFusion();
	testDepthModel();



	return 0;
}