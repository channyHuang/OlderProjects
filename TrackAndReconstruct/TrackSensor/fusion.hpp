/*
#include <gtsam/nonlinear/NonlinearFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>

#include <gtsam/inference/Symbol.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/geometry/Point2.h>

#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/ProjectionFactor.h>
#include <gtsam/slam/GeneralSFMFactor.h>
#include <gtsam/slam/dataset.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/slam/BearingRangeFactor.h>
#include <gtsam/slam/BearingFactor.h>

#include <gtsam/nonlinear/Values.h>
#include <gtsam/base/Vector.h>

#include <gtsam/navigation/ImuBias.h>
#include <gtsam/navigation/ImuFactor.h>
#include <gtsam/navigation/CombinedImuFactor.h>
using namespace gtsam;*/

#include <iostream>
#include <fstream>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "opencv2/gpu/gpu.hpp"

#include "xmlReader.hpp"

#include "../TrackCameraLib/data.h"

#include <QtCore/QString>

int readVisionResult(vector<Eigen::Matrix3d> &R, vector<Eigen::Vector3d> &t) {
	int no = 0;
	int imageNum = image.size();

	for (int i = 0; i < imageNum; i++) {
		R.push_back(Eigen::Matrix3d::Zero());
		t.push_back(Eigen::Vector3d::Zero());
	}

	ifstream fs(DATA_PATH + string("visionparam.txt"));
	string str;
	int tmp = 0;
	char buf[100];
	while (fs) {
		fs >> buf;
		//sscanf(str.c_str(), "Frame %d", &tmp);
		fs >> tmp;

		QString picname = QString(DATA_PATH) + "pic/" + QString("%1").arg(tmp, 4, 10, QChar('0')) + ".jpg";
		imwrite(picname.toStdString(), image.at(tmp));

		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				fs >> R.at(tmp)(x, y);
			}
		}
		for (int y = 0; y < 3; y++) {
			fs >> t.at(tmp)(y);
		}

		no++;

	}

	fs.close();

	return no;
}

int fusion() {
	vector<Eigen::Matrix3d> R; //rotation of vision, load from files
	vector<Eigen::Vector3d> X, Y; //translate vector of imu and vision
	//read from vision result
	int no = readVisionResult(R, Y); // number of frames have vision params
	for (int x = 0; x < R.size(); x++) {
		X.push_back(Vector3d(0, 0, 0));
	}

	string xmlname = (QString(DATA_PATH) + "sensor_3.xml").toStdString();

	//assume these are all know, read from file
	std::vector<sensorData> sensorAccDetail, sensorMagDetail, sensorGyrDetail, sensorRotDetail;
	std::vector<sensorImageData> sensorImageDetail;
	sensorImageDetail.resize(image.size());

	readSensorXMLFile(sensorAccDetail,sensorGyrDetail, sensorMagDetail, sensorRotDetail, xmlname);
	TimeAlignmentVideo(sensorImageDetail, sensorAccDetail, sensorGyrDetail, sensorMagDetail, sensorRotDetail);
	
	cout << "Total " << no << " sensor data will be added to calculate scale." << endl;

	//read sensor data
	Eigen::Matrix3d RB;
	Eigen::Vector3d mB, gB, aB;
	Eigen::Vector3d xf;
	Eigen::Vector3d v_new = Eigen::Vector3d(0, 0, 0), v = Eigen::Vector3d(0, 0, 0);
	double time = sensorImageDetail.at(0).time, lambda = 1, to = 1;
	
	for (int i = 0; i < no; i++) {
		X[i] = v_new*time;

		mB = R[i].col(1); //angle of vision result
		gB = Eigen::Vector3d(sensorImageDetail[i].gyr.R_component_2(), sensorImageDetail[i].gyr.R_component_3(), sensorImageDetail[i].gyr.R_component_4());
		aB = Eigen::Vector3d(sensorImageDetail[i].acc.R_component_2(), sensorImageDetail[i].acc.R_component_3(), sensorImageDetail[i].acc.R_component_4());

		Eigen::Vector3d rzB, ryB, rxB;
		rzB = gB;
		rzB.normalize();
		ryB = rzB.cross(mB);
		ryB.normalize();
		rxB = ryB.cross(rzB);


		RB.col(0) = rxB;
		RB.col(1) = ryB;
		RB.col(2) = rzB;

		//lambda = sum(x*y.t)/sum(y*y.t) 
		double sumxy = 0, sumyy = 0;
		for (int j = 0; j < i; j++) {
			sumxy += X[j].dot(Y[j]);
			sumyy += Y[j].dot(Y[j]);
		}
		lambda = sumxy / sumyy;
		if (sumyy == 0) lambda = 1;

		v_new = v + to*time*RB*(aB - gB);
		v = v_new;

		//final translate vector
		xf = lambda*X[i] + Y[i];
		xf.normalize();

		if (R.at(i)(0, 0) != 0) {
			QString calrtname = QString(DATA_PATH) + "calrt/" + QString("%1").arg(i, 4, 10, QChar('0')) + ".calrt";
			ofstream ofs(calrtname.toStdString());
			ofs << i << endl;
			for (int x = 0; x < 4; x++) {
				ofs << "0" << endl;
			} //distort
			ofs << max(image.at(0).rows, image.at(0).cols) << endl << "0" << endl << image.at(0).rows / 2 << endl << "0" << endl << max(image.at(0).rows, image.at(0).cols) << endl << image.at(0).cols / 2 << endl;
			ofs << "0" << endl << "0" << endl << "1" << endl; //K
			for (int y = 0; y < 3; y++) {
				ofs << xf(y) << endl;
			}
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					ofs << R.at(i)(x, y) << endl;
				}
			}
			ofs.close();
		}
	}

	return lambda;
}