#ifndef FRAMEPARAMS_H
#define FRAMEPARAMS_H

#include <iostream>
#include <fstream>

#include <Eigen/Eigen>

#include <vector>

#define PI 3.14159265897

class CameraParameters {
public:
	CameraParameters() : K(Eigen::Matrix3d::Identity(3, 3)), R(Eigen::Matrix3d::Identity(3, 3)), t(Eigen::Vector3d(0, 0, 0)), P(Eigen::Matrix3Xd::Identity(3, 4)) {}
	CameraParameters(Eigen::Matrix3d _K) : K(_K), R(Eigen::Matrix3d::Identity(3, 3)), t(Eigen::Vector3d(0, 0, 0)), P(Eigen::Matrix3Xd::Identity(3, 4)) {}

	Eigen::Vector3d getC() { return C; }
	Eigen::Matrix3Xd getP() { return P; }
	Eigen::Matrix3d getKinv() { return K_inv; }
	Eigen::Matrix3d getM() { return M; }
	Eigen::Matrix3d getMinv() { return M_inv; }

	void set() {
		setC();
		setP();
		setKinv();
		setMMinv();
	}

	Eigen::Matrix3d R;
	Eigen::Vector3d t;
private:
	Eigen::Matrix3d K;
	
	Eigen::Matrix3d K_inv;
	Eigen::Vector3d C;
	Eigen::Matrix3Xd P;

	Eigen::Matrix3d M;
	Eigen::Matrix3d M_inv;
	
	void setC() { C = -R*t; }
	void setP() { 
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				P(x, y) = R(x, y);
			}
			P(x, 3) = t(x);
		}

		P = K * P;
	}
	void setKinv() { K_inv = K.inverse(); }	
	void setMMinv() {
		M = P.block(0, 0, 3, 3);
		M_inv = M.inverse();
	}
};

class KeyframeSelection {
public:
	std::string dataPath;
	
	void test(std::string path, int ref);
	
	Eigen::Vector3d getViewVector(int refCamIndex, int x, int y);
	Eigen::Vector3d get3Dpoint(int refCamIndex, int x, int y, double depth);

	int referCamIndex;
	//angleDegree * Pi / 180 = angleRadian
	double maxAngleRadian, minAngleRadian, maxAngleDegree, minAngleDegree, maxBaseline, minBaseline;

	std::vector<int> idxOfKeyframe;

	void readParams();

	std::vector<CameraParameters> parameters;
private:

	void selectKeyframes();

	int index; //number of params
};

#endif