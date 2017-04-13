#include "capgSensor/sensor_session.h"

#include <Eigen/Eigen>

#include "MahonyAHRS.h"

#include <iostream>
#include <fstream>
#include <string>

namespace TrackSensor {
	struct imageFrames {
//		cv::Mat imgFrame;
		capg::sensor::Frame accFrame, gyrFrame, rotFrame, magFrame;
		Eigen::Vector3d velocity;
		double timeStep; //from last frame
	};

	class Fusion {
	public:

		capg::sensor::sensor_session_t sensordata;

		std::vector<imageFrames> imgFrames;
		std::vector<imageFrames> tmpImgFrames; // use for filter
		std::vector<Eigen::Vector3d> finalTranslation;

		Eigen::Matrix3d sRc;

		void test(std::string xmlname, std::string videoname, int frameNum);
		//rodrigue to matrix or vector
		void euler2mat(Eigen::Matrix3d &mout, capg::sensor::Frame in);
		void quat2mat(Eigen::Matrix3d &mout, capg::sensor::Frame in);

		void setDatapath(std::string path) { DATA_PATH = path; }

	private:
		bool init(std::string xmlName, std::string videoName, int frameNum); //read raw data

		capg::sensor::Frame BSpine(capg::sensor::Frame a, capg::sensor::Frame b, capg::sensor::Frame c, capg::sensor::Frame d, double t, int index);
		double TimeAlignment();
		void filtering(imageFrames &Frame);

		std::string xmlNameStr, videoNameStr; //xml file name (with path)

		std::vector<capg::sensor::Frame> accData, gyrData, rotData, magData;

		TrackSensor::SensorFilter sensorFilter;

		int numOfFrame; 
		int totalFrame;

		std::string DATA_PATH = "";
		
		void outputRT(std::vector<imageFrames> Frames);
	};
}
