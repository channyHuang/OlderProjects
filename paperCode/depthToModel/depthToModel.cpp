#include "depthToModel.h"

#include <QDir>
#include <QFileInfoList>

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

namespace depthToModel {
	//change single dpethmap to 3d model
	void depthModel::depth2model(int index) {
		cv::Mat_<double> currentDepth = depthmaps.at(index);
		cv::Mat img = imgs.at(index); 
		cv::Mat currentImg = img.clone();
		cv::resize(currentImg, currentImg, cv::Size(currentDepth.cols, currentDepth.rows));

		std::ofstream ofs("points.ply");
		ofs << "ply \n format ascii 1.0 \n element vertex " << currentDepth.rows * currentDepth.cols << " \n property float x \n property float y \n property float z \n property uchar diffuse_red \n property uchar diffuse_green \n property uchar diffuse_blue \n end_header" << std::endl;

		for (int x = 0; x < currentDepth.rows; x++) {
			for (int y = 0; y < currentDepth.cols; y++) {
				Eigen::Vector3d ptX = params.get3Dpoint(index, x, y, currentDepth.at<double>(x, y));

				ofs << ptX(0) << " " << ptX(1) << " " << ptX(2) << " ";
				ofs << (int)currentImg.at<cv::Vec3b>(x, y)[2] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[1] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[0] << std::endl;
			}
		}

		ofs.close();
	}

	//project 3d model with a single view of param
	cv::Mat_<double> depthModel::model2depth(std::string modelName, int index) {
		cv::Mat currentImg = imgs.at(index);
		CameraParameters currentParam = params.parameters.at(index);
		cv::Mat_<double> depth = cv::Mat::zeros(currentImg.rows, currentImg.cols, CV_32F);

		int totalVertex = 0;
		char str[100];
		std::ifstream ifs(modelName);
		ifs >> str;
		while (strcmp(str, "vertex") != 0){
			ifs >> str;
		}
		ifs >> totalVertex;
		while (strcmp(str, "end_header") != 0) {
			ifs >> str;
		}
		
		std::string strs;
		int minn = 99999, maxn = 0;
		while (totalVertex--) {
			Eigen::Vector4d pX = Eigen::Vector4d::Ones(4, 1);
			for (int i = 0; i < 3; i++) {
				ifs >> pX(i, 0);
			}
			std::getline(ifs, strs);
			Eigen::Vector3d px = currentParam.getP() * pX;

			int y = px(0, 0) / px(2, 0);
			int x = px(1, 0) / px(2, 0);
			if (x >= 0 && x < currentImg.rows && y >= 0 && y < currentImg.cols) {
				depth.at<double>(x, y) = px(2, 0);

				minn = std::min(minn, (int)px(2, 0));
				maxn = std::max(maxn, (int)px(2, 0));
			}
		}
		ifs.close();

		for (int i = 0; i < currentImg.rows; i++) {
			for (int j = 0; j < currentImg.cols; j++) {
				depth.at<double>(i, j) = (depth.at<double>(i, j) - minn) * 255 / (maxn - minn);
			}
		}
		//	cv::imwrite("depth.jpg", depth);

		return depth;
	}

	void depthModel::readDepthmaps() {
		int totalNum = params.parameters.size();
		for (int i = 0; i < totalNum; i++) {
			cv::Mat_<double> depth;
			depthmaps.push_back(depth);
		}

		int idx = 0;
		QDir dir((dataPath + "/maskDepthmaps").c_str());
		dir.setFilter(QDir::Files);
		QFileInfoList list = dir.entryInfoList();
		for each (QFileInfo fileinfo in list) {
			std::string name = fileinfo.fileName().toStdString();

			sscanf(name.c_str(), "%d.jpg", &idx);

			cv::Mat_<double> depth = cv::imread(fileinfo.absoluteFilePath().toStdString().c_str(), 0);
			depthmaps.at(idx) = depth;
		}
	}

	void depthModel::readImgs() {
		QDir dir((dataPath + "/maskPic").c_str());
		dir.setFilter(QDir::Files);
		QFileInfoList list = dir.entryInfoList();
		for each (QFileInfo fileinfo in list) {
			cv::Mat img = cv::imread(fileinfo.absoluteFilePath().toStdString().c_str());
			imgs.push_back(img);
		}
	}

	void depthModel::test(std::string dataDir) {
		dataPath = dataDir;

		params.dataPath = dataDir;
		params.readParams();

		readDepthmaps();
		readImgs();
	}

	void depthModel::depthFusion() {
/*		cv::Mat_<double> currentDepth;
		cv::Mat img;
		cv::Mat currentImg;
		char str[100];
		std::ofstream ofs;

		for (int index  = 0; index < depthmaps.size(); index++) {
			if (depthmaps.at(index).rows <=0 ) continue;

			currentDepth = depthmaps.at(index);
			img = imgs.at(index);
			currentImg = img.clone();
			cv::resize(currentImg, currentImg, cv::Size(currentDepth.cols, currentDepth.rows));

			
			sprintf(str, "models/%08d_points.ply", index);
			ofs.open(str);
			ofs << "ply \n format ascii 1.0 \n element vertex " << currentDepth.rows * currentDepth.cols << " \n property float x \n property float y \n property float z \n property uchar diffuse_red \n property uchar diffuse_green \n property uchar diffuse_blue \n end_header" << std::endl;

			for (int x = 0; x < currentDepth.rows; x++) {
				for (int y = 0; y < currentDepth.cols; y++) {
					Eigen::Vector3d ptX = params.get3Dpoint(index, x, y, currentDepth.at<double>(x, y));

					ofs << ptX(0) << " " << ptX(1) << " " << ptX(2) << " ";
					ofs << (int)currentImg.at<cv::Vec3b>(x, y)[2] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[1] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[0] << std::endl;
				}
			}
			ofs.close();
		}
*/
#if 1
		int index = 3;
		cv::Mat_<double> currentDepth = depthmaps.at(index);
		cv::Mat img = imgs.at(index);
		cv::Mat currentImg = img.clone();
		cv::resize(currentImg, currentImg, cv::Size(currentDepth.cols, currentDepth.rows));

		std::ofstream ofs("points.ply");
		ofs << "ply \n format ascii 1.0 \n element vertex " << currentDepth.rows * currentDepth.cols << " \n property float x \n property float y \n property float z \n property uchar diffuse_red \n property uchar diffuse_green \n property uchar diffuse_blue \n end_header" << std::endl;

		for (int x = 0; x < currentDepth.rows; x++) {
			for (int y = 0; y < currentDepth.cols; y++) {
				Eigen::Vector3d ptX = params.get3Dpoint(index, x, y, currentDepth.at<double>(x, y));

				ofs << ptX(0) << " " << ptX(1) << " " << ptX(2) << " ";
				ofs << (int)currentImg.at<cv::Vec3b>(x, y)[2] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[1] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[0] << std::endl;
			}
		}
		/*
		index = 50;
		currentDepth = depthmaps.at(index);
		img = imgs.at(index);
		currentImg = img.clone();
		cv::resize(currentImg, currentImg, cv::Size(currentDepth.cols, currentDepth.rows));

		Eigen::Matrix3d betweenR = params.parameters.at(index).R.inverse() * params.parameters.at(3).R;

		for (int x = 0; x < currentDepth.rows; x++) {
			for (int y = 0; y < currentDepth.cols; y++) {
				Eigen::Vector3d ptX = params.get3Dpoint(3, x, y, currentDepth.at<double>(x, y));
				ptX = betweenR * ptX;

				ofs << ptX(0) << " " << ptX(1) << " " << ptX(2) << " ";
				ofs << (int)currentImg.at<cv::Vec3b>(x, y)[2] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[1] << " " << (int)currentImg.at<cv::Vec3b>(x, y)[0] << std::endl;
			}
		}

		ofs.close();*/
#endif
	}
}