#ifndef DEPTHTOMODEL_H
#define DEPTHTOMODEL_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "KeyframeSelection/frameParams.h"

namespace depthToModel {
	class depthModel {
	public:
		void readDepthmaps();
		void readImgs();

		void depthFusion();

		void depth2model(int index);
		cv::Mat_<double> model2depth(std::string modelName, int index);

		void test(std::string dataPath);

	private:
		std::string dataPath;

		std::vector<cv::Mat_<double>> depthmaps;
		std::vector<cv::Mat> imgs;
		KeyframeSelection params;
	};
}

#endif