#ifndef SURFEL_H
#define SURFEL_H

#include <QtGui/QImage>
#include <QtGui/QColor>

#include <Eigen/Eigen>

#include <iostream>
#include <vector>
using namespace std;

#include "vectorimage.hpp"
#include "camera.hpp"

//one point of 3D
class Surfel
{
public:
	Eigen::Vector3d Point;//position
	Eigen::Vector3d Normal;//normal vector
	RGBA color; //color
	double confScore; //confidence score = weightIn - weightOut
	double weightIn, weightOut; //weight

	//threshold
	double alpha_max, lamda, thr;

	void init(Eigen::Vector3d nx, Eigen::Vector3d vx, double nccCost) 
	{
		alpha_max = 80;
		lamda = 5.0;
		thr = 0.65;

		weightIn = GeometryWeight(nx, vx) * CameraWeight() * PhotoconsistencyWeight(nccCost);
		weightOut = 0;
		confScore = weightIn - weightOut;
	}

private:
	double GeometryWeight(Eigen::Vector3d nx, Eigen::Vector3d vx) {
		double dotResult = nx.dot(vx);
		double angle = acos(dotResult / (nx.norm() * vx.norm()) );

		assert( abs(nx.norm() * vx.norm()*cos(angle) - dotResult) <= 1e-9);

		if (angle <= alpha_max) {
			return (dotResult - cos(alpha_max) / (1 - cos(alpha_max)));
		}
		else { return 0; }
	}

	double CameraWeight() {
		return 1 - exp(-lamda);
	}

	double PhotoconsistencyWeight(double nccCost) {
		double nccNum = nccCost;
		if (nccNum >= thr) { return nccNum; }
		else { return 0; }
	}
};

//all points of 3D
class SurfelOperation
{
public:
	vector<Surfel> surfels; //3d points
	vector<VectorImage> newDepths; //depth maps

	void init() { initialize = false; }

	//create a new surfel
	Surfel createSurfel(Camera cmaeraView, Eigen::Vector3d point, RGBA color, double nccCost);
	void Adding(Surfel surfel);
	void Removing(Surfel surfel);

	void surfelOpeLine(Camera cmaeraView, Eigen::Vector3d point, RGBA color, double nccCost);
private:
	bool initialize;
};


#endif