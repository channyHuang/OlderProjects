#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>



//Mine
#include "convertAhandaPovRayToStandard.h"
#include "CostVolume/utils/reproject.hpp"
#include "CostVolume/utils/reprojectCloud.hpp"
#include "CostVolume/Cost.h"
#include "CostVolume/CostVolume.hpp"
#include "Optimizer/Optimizer.hpp"
#include "DepthmapDenoiseWeightedHuber/DepthmapDenoiseWeightedHuber.hpp"
//#include "OpenDTAM.hpp"
//#include "graphics.hpp"
#include "set_affinity.h"
#include "Track/Track.hpp"

#include "utils/utils.hpp"


//debug
#include "tictoc.h"

#include <QtCore/QDir>
#include <cstring>
#include <fstream>

#include "sensorFusion/fusion.h"

const static bool valgrind = 0;

//A test program to make the mapper run
using namespace cv;
using namespace cv::gpu;
using namespace std;

int main(int argc, char** argv)
{
	argc = 2;
	argv[1] = "E:/newData";

	if (argc < 2) {
		std::cout << "Sorry, params not enough" << std::endl;
		return 0;
	}

	ofstream of("message.txt");
	//	streambuf *streams = cout.rdbuf();
	//	cout.rdbuf(of.rdbuf());
#if 0
	//数据预处理
	TrackSensor::Fusion fusion;

	std::string videoName = "E:/mycode/videodata/VID_20150929_192134/VID_20150929_192134.mp4";
	std::string xmlName = "E:/mycode/videodata/VID_20150929_192134/sensor_0.xml";

	fusion.test(xmlName, videoName, 50);
#endif

	//数据准备
	std::string picPath = argv[1];

/*	cv::Mat cameraMatrix = (Mat_<double>(3, 3) << 481.20, 0.0, 319.5,
		0.0, 480.0, 239.5,
		0.0, 0.0, 1.0); //摄像机内参K*/

	cv::Mat cameraMatrix = (Mat_<double>(3, 3) << 1410, 0, 800,
		0, 1410, 600,
		0, 0, 1);


	QDir dir((picPath + "/pic").c_str());
	dir.setFilter(QDir::Files);
	QFileInfoList list = dir.entryInfoList();

	int numImg = list.size();
	std::cout << "Total images: " << list.size() << std::endl;

	std::vector<cv::Mat> images, Rs, Ts, Rs0, Ts0;
	cv::Mat ret;
	int idx = 0;
	
	cv::Mat R, T;

	for each (QFileInfo fileinfo in list) {
		string imgName = fileinfo.absoluteFilePath().toStdString().c_str();
		//读取估计的外参
		cv::Mat tmpimg = cv::imread(imgName, -1);
		int reRow = tmpimg.rows / 32 * 32, reCol = tmpimg.cols / 32 * 32;
		cv::resize(tmpimg, tmpimg, cv::Size(reCol, reRow));
		tmpimg.convertTo(tmpimg, CV_32FC3, 1.0 / 65535.0);
		images.push_back(tmpimg);
	}

	QDir ndir((picPath + "/calrt").c_str());
	ndir.setFilter(QDir::Files);
	list = ndir.entryInfoList();
	for each (QFileInfo fileinfo in list) {
		R = Mat(3, 3, CV_64F);
		T = Mat(3, 1, CV_64F);
		std::ifstream ifs(fileinfo.absoluteFilePath().toStdString().c_str());
		for (int xx = 0; xx < 3; xx++) {
			for (int yy = 0; yy < 3; yy++) {
				ifs >> R.at<double>(xx, yy);
			}
		}
		for (int xx = 0; xx < 3; xx++) {
			ifs >> T.at<double>(xx);
		}
		ifs.close();		

		Rs.push_back(R.clone());
		Ts.push_back(T.clone());
		Rs0.push_back(R.clone());
		Ts0.push_back(T.clone());
	}
	/*
	QDir ndir("E:/newData/sfmResult/visualSFM.nvm.cmvs/00/txt/");
	ndir.setFilter(QDir::Files);
	list = ndir.entryInfoList();
	char tmpstr[100];
	for each (QFileInfo fileinfo in list) {
		std::ifstream ifs(fileinfo.absoluteFilePath().toStdString().c_str());
		ifs >> tmpstr;
		cv::Mat P = Mat(3, 4, CV_64F);
		for (int xx = 0; xx < 3; xx++) {
			for (int yy = 0; yy < 4; yy++) {
				ifs >> P.at<double>(xx, yy);
			}
		}
		ifs.close();
		cv::Mat RT = Mat(3, 4, CV_64F);
		RT = cameraMatrix.inv()*P;
		R = RT(Range(0,3), Range(0, 3));
		T = RT(Range(0, 3), Range(3, 4));

		Rs.push_back(R.clone());
		Ts.push_back(T.clone());
		Rs0.push_back(R.clone());
		Ts0.push_back(T.clone());
	}
	*/

	std::cout << "params size: " << Rs0.size() << " " << Ts0.size() << std::endl;

	/*cv::Mat R, T;
	char filename[500];
	double reconstructionScale = 5 / 5.;
	numImg = 50;
	for (int i = 0; i < numImg; i++){
		Mat tmp;
		sprintf(filename, "E:/thirdLib/opendtam_cross_platform-master/build/Trajectory_30_seconds/scene_%03d.png", i);
		convertAhandaPovRayToStandard("E:/thirdLib/opendtam_cross_platform-master/build/Trajectory_30_seconds",
			i,
			cameraMatrix,
			R,
			T);
		Mat image;
		cout << "Opening: " << filename << endl;

		imread(filename, -1).convertTo(image, CV_32FC3, 1.0 / 65535.0);
		resize(image, image, Size(), reconstructionScale, reconstructionScale);

		images.push_back(image.clone());
		Rs.push_back(R.clone());
		Ts.push_back(T.clone());
		Rs0.push_back(R.clone());
		Ts0.push_back(T.clone());
	}*/

	ret = CudaMem(images[0].rows, images[0].cols, CV_32FC1).createMatHeader();

	//外参数修正
	int layers = 32;
	int imagesPerCV = 10;
	CostVolume cv(images[0], (FrameID)0, layers, 0.015, 0.0, Rs[0], Ts[0], cameraMatrix);

	cv::Mat image;//, R, T;
	cv::gpu::Stream s;
	int inc = 1;

	for (int imageNum = 1; imageNum < numImg; imageNum++){
		std::cout << "Deal with image number " << imageNum << ":" << std::endl;

		if (inc == -1 && imageNum < 4){
			inc = 1;
		}
		T = Ts[imageNum].clone();
		R = Rs[imageNum].clone();
		image = images[imageNum];

		if (cv.count < imagesPerCV){
			cv.updateCost(image, R, T);
			cudaDeviceSynchronize();
		}
		else {
			cudaDeviceSynchronize();
			//Attach optimizer
			Ptr<DepthmapDenoiseWeightedHuber> dp = createDepthmapDenoiseWeightedHuber(cv.baseImageGray, cv.cvStream);
			DepthmapDenoiseWeightedHuber& denoiser = *dp;
			Optimizer optimizer(cv);
			optimizer.initOptimization();
			GpuMat a(cv.loInd.size(), cv.loInd.type());
			//             cv.loInd.copyTo(a,cv.cvStream);
			cv.cvStream.enqueueCopy(cv.loInd, a);
			GpuMat d;
			denoiser.cacheGValues();
			ret = image * 0;

			cv.loInd.download(ret);
			bool doneOptimizing; int Acount = 0; int QDcount = 0;
			do{
				a.download(ret);
				for (int i = 0; i < 10; i++) {
					d = denoiser(a, optimizer.epsilon, optimizer.getTheta());
					QDcount++;
					d.download(ret);
				}
				doneOptimizing = optimizer.optimizeA(d, a);
				Acount++;
			} while (!doneOptimizing);
			optimizer.cvStream.waitForCompletion();
			a.download(ret);

			Track tracker(cv);
			Mat out = optimizer.depthMap();

			double m;
			minMaxLoc(out, NULL, &m);
			tracker.depth = out*(.66*cv.near / m);

			if (imageNum + imagesPerCV + 1 >= numImg){
				inc = -1;
			}
			imageNum = imageNum - imagesPerCV + 1 - inc;
			for (int i = imageNum; i < numImg&&i <= imageNum + imagesPerCV + 1; i++){
				tracker.addFrame(images[i]);
				tracker.align();
				LieToRT(tracker.pose, R, T);
				Rs[i] = R.clone();
				Ts[i] = T.clone();

				Mat p, tp;
				p = tracker.pose;
				tp = RTToLie(Rs0[i], Ts0[i]);

				if (0) {//debug
					cout << "True Pose: " << tp << endl;
					cout << "True Delta: " << LieSub(tp, tracker.basePose) << endl;
					cout << "Recovered Pose: " << p << endl;
					cout << "Recovered Delta: " << LieSub(p, tracker.basePose) << endl;
					cout << "Pose Error: " << p - tp << endl;
				}
				//cout << i << endl;
				//cout << Rs0[i] << Rs[i];

				reprojectCloud(images[i], images[cv.fid], tracker.depth, RTToP(Rs[cv.fid], Ts[cv.fid]), RTToP(Rs[i], Ts[i]), cameraMatrix);
			}

			optimizer.cvStream.waitForCompletion();
			cv::Mat depthMap = optimizer.depthMap();
			double min, max;
			cv::minMaxLoc(depthMap, &min, &max);
			std::cout << "Max: " << max << " Min: " << min << std::endl;
			cv::Mat depthImg;
			depthMap.convertTo(depthImg, CV_8U, 255 / max, 0);
			medianBlur(depthImg, depthImg, 3);
			char str[100];
			sprintf(str, "E:/depthMaps/%08d.jpg", imageNum);
			cv::imwrite(str, depthImg);

			cv = CostVolume(images[imageNum], (FrameID)imageNum, layers, 0.015, 0.0, Rs[imageNum], Ts[imageNum], cameraMatrix);
			s = optimizer.cvStream;
			a.download(ret);
		}

		s.waitForCompletion();// so we don't lock the whole system up forever
	}
	s.waitForCompletion();
	Stream::Null().waitForCompletion();

	//	cout.rdbuf(streams);

	char str[100];
	for (int i = 0; i < numImg; i++) {
		sprintf(str, "E:/newData/newCalrt/%08d.txt", i);
		std::ofstream ofs(str);
		for (int xx = 0; xx < 3; xx++) {
			for (int yy = 0; yy < 3; yy++) {
				ofs << Rs[i].at<double>(xx, yy) << " ";
			}
			ofs << std::endl;
		}
		for (int yy = 0; yy < 3; yy++) {
			ofs << Ts[i].at<double>(yy) << " ";
		}
		ofs << std::endl;
		ofs.close();
	}

	of.close();

	return 0;
}


