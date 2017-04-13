#include "frameParams.h"

#include <QDir>
#include <QFileInfoList>

void KeyframeSelection::readParams() {	
	std::string intrincPath = dataPath + "/K.txt";
	std::ifstream ifs;
	
	Eigen::Matrix3d sameK = Eigen::Matrix3d::Identity(3, 3);
	ifs.open(intrincPath);
	for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				ifs >> sameK(x, y);
			}
		}
	ifs.close();
	
	std::string paramPath = dataPath + "/newCalrt";
	
	QDir dir(paramPath.c_str());
	dir.setFilter(QDir::Files);
	QFileInfoList list = dir.entryInfoList();
	
	int totalNum = list.size();
	for (int x = 0; x < totalNum; x++) {
		parameters.push_back(CameraParameters(sameK));
	}
	
	index = 0;
	for each (QFileInfo fileinfo in list) {
		std::string paramName = fileinfo.absoluteFilePath().toStdString().c_str();
		
		ifs.open(paramName);
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				ifs >> parameters.at(index).R(x, y);
			}
		}
		for (int x = 0; x < 3; x++) {
			ifs >> parameters.at(index).t(x);
		}
		ifs.close();
		parameters.at(index).set();
		index++;
	}
}

void KeyframeSelection::selectKeyframes() {

	int imgx = 0; 
	int imgy = 0;

	Eigen::Vector3d refVec = getViewVector(referCamIndex, imgx, imgy);

	for (int x = 0;x < parameters.size(); x++) {
		Eigen::Vector3d vec = getViewVector(x, imgx, imgy);
		double baseline = (parameters.at(referCamIndex).getC() - parameters.at(x).getC()).norm();
		double angle = acosf(vec.dot(refVec)) ;

		if (angle != angle) angle = 0;

		//threshold of baseline and angle of current frame and reference frame

		std::cout << "Accepting camera angle range [" << minAngleDegree << " - " << minAngleDegree << "], baseline range [" << " - " << "]" << std::endl;

		if (baseline > 0 && angle > minAngleRadian && angle < maxAngleRadian) {
			idxOfKeyframe.push_back(x);
		}
	}
}

Eigen::Vector3d KeyframeSelection::getViewVector(int refCamIndex, int x, int y) {
	Eigen::Vector3d vec = get3Dpoint(refCamIndex, x, y, 1.0) - parameters.at(refCamIndex).getC();
	vec.normalize();
	return vec;
}

Eigen::Vector3d KeyframeSelection::get3Dpoint(int refCamIndex, int x, int y, double depth) {
	Eigen::Vector3d pt = Eigen::Vector3d(x, y, 1);
	Eigen::Vector3d ptX = parameters.at(refCamIndex).getMinv() * (depth * pt - parameters.at(refCamIndex).getP().col(3));

	return ptX;
}

void KeyframeSelection::test(std::string path, int ref) {
	dataPath = path;
	referCamIndex = ref;

	maxAngleDegree = 45;
	minAngleDegree = 5;

	maxAngleRadian = maxAngleDegree * 180 / PI;
	minAngleRadian = minAngleDegree * 180 / PI;

	readParams();

	if (ref >= index) {
		std::cout << "Error: reference camera index is bigger than total number of params..." << std::endl;
		exit(0);
	}

	selectKeyframes();
}