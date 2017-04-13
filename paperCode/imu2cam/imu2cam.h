#ifndef IMU2CAM_H
#define IMU2CAM_H

#include "KeyframeSelection/frameParams.h"
#include "capgSensor/sensor_session.h"

namespace crank {
	class imu2cam {
	public:
		void test(std::string dataDir);
	private:
		std::string dataPath;
		int numOfFrame;

		KeyframeSelection params;
		capg::sensor::sensor_session_t sensordata;

		bool readImuParams(std::string xmlNameStr);

		Eigen::VectorXcd computeImuCamRotation();

		std::vector<capg::sensor::Frame> accData;
	};
}

#endif