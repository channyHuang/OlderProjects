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

const static bool valgrind=0;

//A test program to make the mapper run
using namespace cv;
using namespace cv::gpu;
using namespace std;

int main( int argc, char** argv )
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

	cv::Mat cameraMatrix = (Mat_<double>(3,3) << 481.20,0.0,319.5,
		0.0,480.0,239.5,
		0.0, 0.0, 1.0); //摄像机内参K

	QDir dir((picPath + "/pic").c_str());
	dir.setFilter(QDir::Files);
	QFileInfoList list = dir.entryInfoList();

	int numImg = list.size();
	std::cout << "Total images: " << list.size() << std::endl;

	std::vector<cv::Mat> images, Rs, Ts, Rs0, Ts0;
	cv::Mat ret;
	int idx = 0;
	/*
	cv::Point3d direction = cv::Point3d(0, 0, 1), upvector = cv::Point3d(0, 1, 0), posvector;
	cv::Mat R, T;

	for each (QFileInfo fileinfo in list) {
//		idx++;
		string imgName = fileinfo.absoluteFilePath().toStdString().c_str();
//		std::cout << "Image number " << idx << ", of total " << numImg << ": " << imgName << std::endl;

		//读取估计的外参
		cv::Mat tmpimg = cv::imread(imgName, -1);
		int reRow = tmpimg.rows / 32 * 32, reCol = tmpimg.rows / reRow * tmpimg.cols;
		cv::resize(tmpimg, tmpimg, cv::Size(reRow, reCol));
		tmpimg.convertTo(tmpimg, CV_32FC3, 1.0 / 65535.0);
		images.push_back(tmpimg);
	}

	QDir ndir((picPath + "/calrt").c_str());
	ndir.setFilter(QDir::Files);
	list = ndir.entryInfoList();
	for each (QFileInfo fileinfo in list) {
		std::ifstream ifs(fileinfo.absoluteFilePath().toStdString().c_str());
		ifs >> posvector.x >> posvector.y >> posvector.z;
		ifs.close();
		R = Mat(3, 3, CV_64F);
		R.row(0) = Mat(direction.cross(upvector)).t();
		R.row(1) = Mat(-upvector).t();
		R.row(2) = Mat(direction).t();
		T = R*Mat(posvector);

		Rs.push_back(R.clone());
		Ts.push_back(T.clone());
		Rs0.push_back(R.clone());
		Ts0.push_back(T.clone());
	}
	std::cout << "params size: " << Rs0.size() << " " << Ts0.size() << std::endl;*/
	
	cv::Mat R, T;
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
	}

	ret = CudaMem(images[0].rows, images[0].cols, CV_32FC1).createMatHeader();

	//外参数修正
	int layers = 32;
	int imagesPerCV = 20;
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

			std::cout << cost.size() << " " << cost.type() << std::endl;

			cv::waitKey(0);

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


