#ifndef READANDRUN_H
#define READANDRUN_H

#include <iostream>
#include <fstream>
#include <string>
#include "stdio.h"
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "opencv2/gpu/gpu.hpp"

#include "../TrackCameraLib/data.h"
#include "../TrackCameraLib/MultiCameraPnP.h"

#include "../ReconstructLib/twoviewstereo.hpp"
#include "../ReconstructLib/multiviewstereo.hpp"
#include "../ReconstructLib/camera.hpp"

#include <QtCore/QDateTime>
#include <Eigen/Eigen>
#include "../TrackSensor/fusion.hpp"

bool readVideo(string videoname)
{
	cv::VideoCapture capture;
	capture.open(videoname);
	if (!capture.isOpened()) {
		cout << "Could not open video" << videoname << "with opencv..." << endl;
		return false;
	}

	cv::Mat frame;

	int no = 0;
	for (;;) {
		capture >> frame;
		if (frame.empty())
			break;
		if (no % 30 == 0) {
			image.push_back(frame.clone());
		}
		no++;
	}
	if (image.size() == 0) {
		cerr << "Can't get image frames from video..." << endl;
		return false;
	}
	cout << "Total frams: " << image.size() << " will be added to calculate the camera parameters." << endl;

	return true;
}

int mainTrack() {
	string videoName = (QString(DATA_PATH) + "sensor3.mp4").toStdString();

	readVideo(videoName);
	cv::Ptr<MultiCameraPnP> distance = new MultiCameraPnP(image);
	distance->use_rich_features = true;
	distance->use_gpu = (cv::gpu::getCudaEnabledDeviceCount() > 0);
	distance->RecoverDepthFromImages();

	return 0;
}

void readCameraParam(Camera &leftview)
{
	ifstream fs(leftview.name().toStdString());
	int no;
	fs >> no;
	Eigen::Matrix3d K, R;
	Eigen::Vector3d T;
	LensDistortions distort;
	for (int i = 0; i < 4; i++) {
		fs >> distort.at(i);
	}
	distort.at(4) = 0.0;
	leftview.setLensDistortion(distort);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			fs >> K(i, j);
		}
	}
	for (int i = 0; i < 3; i++) {
		fs >> T(i);
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			fs >> R(i, j);
		}
	}
	leftview.set(K, R, T);
}

void readMaskImages(vector<QString> &masknames)
{
	QDir dir(DATA_PATH + QString("mask"));
	dir.setFilter(QDir::Files);
	QFileInfoList list = dir.entryInfoList();

	for each (QFileInfo fileinfo in list)
	{
		if (!fileinfo.isFile()) continue;
		if (0 == fileinfo.suffix().compare("bmp") || 0 == fileinfo.suffix().compare("jpg"))
		{
			masknames.push_back(fileinfo.absoluteFilePath());
		}
	}
}

void readimageset(std::vector<QString> &imagenames, std::vector<Camera> &views) {
	QDir dir(DATA_PATH + QString("pic"));
	dir.setFilter(QDir::Files);
	QFileInfoList list = dir.entryInfoList();

	for each (QFileInfo fileinfo in list)
	{
		if (!fileinfo.isFile()) continue;
		if (0 == fileinfo.suffix().compare("bmp") || 0 == fileinfo.suffix().compare("jpg"))
		{
			imagenames.push_back(fileinfo.absoluteFilePath());
		}
	}

	QDir pdir(DATA_PATH + QString("calrt"));
	pdir.setFilter(QDir::Files);
	list = pdir.entryInfoList();
	int i = 0;
	for each (QFileInfo fileinfo in list)
	{
		if (!fileinfo.isFile()) continue;
		if (0 == fileinfo.suffix().compare("txt") || 0 == fileinfo.suffix().compare("calrt"))
		{
			Camera view(QString::number(i), fileinfo.absoluteFilePath());
			readCameraParam(view);
			views.push_back(view);
		}
		i++;
	}
}

int mainReconstruct() {
	MultiViewStereo stereo;
	std::vector<Camera> views;
	std::vector<QString> imagenames, masknames;

	readimageset(imagenames, views);
	cout << "Total " << imagenames.size() << " will be added to reconsturct 3d scene." << endl;

	readMaskImages(masknames);

	stereo.initialize(imagenames, masknames, views, 0, 255, 256, 0.1, 0.1);
	stereo.runTask();

	QDateTime time = QDateTime::currentDateTime();
	QString timename = time.toString("dd_hh_mm_ss");

	cout << "Finished!" << endl;

	return 0;
}

int mainReconstruct2() {

	std::vector<Camera> views;
	std::vector<QString> imagenames, masknames;

	readimageset(imagenames, views);
	cout << "Total " << imagenames.size() << " will be added to reconsturct 3d scene." << endl;

	readMaskImages(masknames);

	QImage orgImg, dstImg, orgMask, dstMask;
	std::vector<PLYPoint> totalPoints;
	for (int x = 1; x < imagenames.size(); x++) {
		orgImg.load(imagenames[x - 1]);
		dstImg.load(imagenames[x]);
		if (masknames.size() == imagenames.size()) {
			orgMask.load(masknames[x - 1]);
			dstMask.load(masknames[x]);
		}
		else {
			orgMask = QImage();
			dstMask = QImage();
		}

		TwoViewStereo stereo(views[0], orgImg, orgMask, views[x], dstImg, dstMask, 0, 255, 256, 0.1);

		stereo.limg = cv::Mat(orgImg.height(), orgImg.width(), CV_8UC4, (uchar*)orgImg.bits(), orgImg.bytesPerLine());
		stereo.rimg = cv::Mat(dstImg.height(), dstImg.width(), CV_8UC4, (uchar*)dstImg.bits(), dstImg.bytesPerLine());
		stereo.computeDepthMaps();

		for (int y = 0; y < stereo.threeDpoints.size(); y++) {
			Eigen::Vector3d tmppoint = views[x - 1].fromLocalToGlobal(stereo.threeDpoints[y].first);
			tmppoint = views[0].fromGlobalToLocal(tmppoint);
			totalPoints.push_back(PLYPoint(tmppoint, stereo.threeDpoints[y].second));
		}

		QDateTime time = QDateTime::currentDateTime();
		QString timename = time.toString("dd_hh_mm_ss");
		stereo.rightDepthMap().save(DATA_PATH + timename + "_" + QString::number(x) + ".jpg");
	}
	string plypath = DATA_PATH + string("3dpoint.ply");
	outputPLYFile(plypath, totalPoints);

	return 0;
}

#endif