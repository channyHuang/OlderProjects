#include "updateCostVolumn.h"

namespace UpdateFromDepth {
	Volumetrix::Volumetrix(cv::Mat _image, int _layers, float _near, float _far) : image(_image), layers(_layers), near(_near), far(_far) {
		rows = image.rows;
		cols = image.cols;
		depthStep = (near - far) / (layers - 1);
		
		int wi = 1;
		dataContainer.create(layers, rows * cols, CV_32FC1);
		
		W.create(layers, rows * cols, CV_32FC1);
		cvStream.enqueueMemSet(W,wi);
	}
	
	Eigen::vector3d p2q(Eigen::vector3d p) {
		return K*R*p;
	}
	
	Eigen::vector2d pi(Eigen::vector3d q) {
		Eigen::vector2d piq;
		piq(0) = K.at<double>(0, 0) * q(0) / q(2) + K.at<double>(0, 2);
		piq(1) = K.at<double>(1, 1) * q(0) / q(2) + K.at<double>(1, 2);
		return piq
	}
	
	Volumetrix::refine(depth, tmpimage, tmpR, tmpT) {
		Eigen::vector3d q = p2q(p);
		Eigen::vector2d piq = pi(q);
		
		si = depth.at<uchar>(piq(0), piq(1)) - q(2);
		ci = image.at<vec3>(piq(0), piq(1));
		
		Si = Si * Wi + si*wi;
		Ci = Ci * Wi + ci*wi;
		Wi = Wi + wi;
	}
}