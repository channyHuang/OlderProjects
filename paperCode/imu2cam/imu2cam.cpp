#include "imu2cam.h"

#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>

namespace crank {
	bool imu2cam::readImuParams(std::string xmlNameStr) {
		if (!sensordata.load(xmlNameStr)) return false;

		auto node = sensordata.get_node("0");
		auto acce = *node.channel("a");

		int frameStep = acce.frame_count() / numOfFrame;
		for (int i = 0; i < acce.frame_count(); i+= frameStep) {
			accData.push_back(acce.frame(i));
		}
	}
	
	Eigen::VectorXcd imu2cam::computeImuCamRotation() {
		std::vector<Eigen::Vector3d> imus, cams;
		for (int i =0;i<numOfFrame;i++) {
			Eigen::Vector3d cam = params.parameters.at(i).R.col(2);
			cams.push_back(cam);

			Eigen::Vector3d imu = Eigen::Vector3d(accData.at(i).x, accData.at(i).y, accData.at(i).z);
			imus.push_back(imu);
		}

		Eigen::Matrix3d S = Eigen::Matrix3d::Zero(3, 3);
		for (int idx = 0; idx < numOfFrame; idx++) {
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					S(x, y) = S(x, y) + imus.at(idx)(x)*cams.at(idx)(y);
				}
			}
		}

		Eigen::Matrix4d A = Eigen::Matrix4d::Zero(4, 4);
		A(0, 0) = S(0, 0) + S(1, 1) + S(2, 2);
		A(1, 1) = S(0, 0) - S(1, 1) - S(2, 2);
		A(2, 2) = -S(0, 0) + S(1, 1) - S(2, 2);
		A(3, 3) = -S(0, 0) - S(1, 1) + S(2, 2);

		A(0, 1) = A(1, 0) = S(1, 2) - S(2, 1);
		A(0, 2) = A(2, 0) = S(2, 0) - S(0, 2);
		A(0, 3) = A(3, 0) = S(0, 1) - S(1, 0);

		A(1, 2) = A(2, 1) = S(0, 1) + S(1, 0);
		A(1, 3) = A(3, 1) = S(2, 0) + S(0, 2);
		A(2, 3) = A(3, 2) = S(1, 2) + S(2, 1);

		Eigen::EigenSolver<Eigen::Matrix4d> eig(A);// [vec val] = eig(A)

		int idx = 0;
		
		Eigen::VectorXcd D = eig.eigenvalues();
		double maxn = abs(D(0));
		for (int i = 1; i < 4; i++) {
			if (maxn < abs(D(i)) ){
				maxn = abs(D(i));
				idx = i;
			}
		}
		
		Eigen::VectorXcd r = eig.eigenvectors().col(idx);
	
		return r;
	}

	void imu2cam::test(std::string dataDir) {
		dataPath = dataDir;

		params.readParams();
		numOfFrame = params.parameters.size();

		std::string xmlNameStr = dataDir + "/sensor_0.xml";
		readImuParams(xmlNameStr);

		computeImuCamRotation();
	}
}