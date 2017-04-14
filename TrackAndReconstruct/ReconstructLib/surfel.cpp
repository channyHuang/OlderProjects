#include "surfel.h"

#include <iostream>
using namespace std;

Surfel SurfelOperation::createSurfel(Camera cameraView, Eigen::Vector3d point, RGBA color, double nccCost)
{
	Surfel surfel;
	surfel.Point = point;
	surfel.color = color;
	surfel.Normal = Eigen::Vector3d(0, 0, 0);

	Eigen::Vector3d nx, vx;

	nx = cameraView.R().col(2);
	vx = point;
	vx.normalize();

	surfel.init(nx, vx, nccCost);

	return surfel;
}

void SurfelOperation::Adding(Surfel surfel)
{
	surfels.push_back(surfel);
}

void SurfelOperation::Removing(Surfel surfel)
{
	for (vector<Surfel>::iterator it = surfels.begin(); it != surfels.end(); it++)	{ 
		if (it->Point == surfel.Point) {
			surfels.erase(it);
			break;
		}
	}
	
}

void SurfelOperation::surfelOpeLine(Camera cmaeraView, Eigen::Vector3d point, RGBA color, double nccCost)
{
	/*if (newDepths.size() <= 0) {
		cout << "Error in SurfelOperation.surfelOpeLine:\n Size of depth maps is wrong!" << endl;
		return;
	}*/
	double thresholdValue = -1e9;// -0.5;
	double epson = 1e-9;
	Surfel surfel = createSurfel(cmaeraView, point, color, nccCost);
	//remove
	if (surfel.confScore < thresholdValue) {
		Removing(surfel);
		return;
	}

	vector<Surfel>::iterator it;
	//for each surfel in the list
	for (it = surfels.begin(); it != surfels.end(); it++)	{	
		//update
		Eigen::Vector3d curSur = it->Point;
		curSur.normalize();
		Eigen::Vector3d objSur = surfel.Point;
		objSur.normalize();
		if (curSur == objSur) {
			double dx = it->Point.z();
			double dpj = surfel.Point.z();
			//d(pj) >> dx, do nothing
			if (dpj > dx * 10) {
				;
			}
			//d(pj) << dx, weightOut = weightOut + w(x)
			else if (dpj < dx / 10) {
				it->weightOut += surfel.weightIn;
			}
			// < epson
			else if (abs(dpj - dx) / dpj < epson) {
				// angle < 45
				if (acos(surfel.Normal.dot(surfel.Point)) <= 45) {
					it->weightIn += surfel.weightIn;
				}
				// angle > 45
				else {
					it->weightOut += surfel.weightIn;
				}
			}
			break;
		}
	}
	if (it == surfels.end()) {
		Adding(surfel);
	}

}