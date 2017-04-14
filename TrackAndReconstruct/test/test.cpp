#include<iostream>
using namespace std;

#include "../TrackAndReconstruct/readAndRun.hpp"

#include "../ReconstructLib/twoviewstereo.hpp"

void test(int i) {

	switch (i) {
	case 0: 
		break;
	case 1: //testReconstruct
		mainReconstruct();
		break;
	case 2: //testTracking
	{
		image.clear();
		for (int x = 0; x < 12; x++) {
			string testimg = (QString(DATA_PATH) + "pic/" + QString("%1").arg(x + 1, 4, 10, QChar('0')) + ".jpg").toStdString();
			image.push_back(cv::imread(testimg));
		}

		cv::Ptr<MultiCameraPnP> distance = new MultiCameraPnP(image);
		distance->use_rich_features = true;
		distance->use_gpu = (cv::gpu::getCudaEnabledDeviceCount() > 0);
		distance->RecoverDepthFromImages();
	}
		break;
	case 3: // test using two to construct all
	{
		
		std::vector<Camera> views;
		std::vector<QString> imagenames, masknames;

		readimageset(imagenames, views);
		cout << "Total " << imagenames.size() << " will be added to reconsturct 3d scene." << endl;

		readMaskImages(masknames);

		QImage orgImg, dstImg, orgMask, dstMask;

		std::vector<PLYPoint> totalPoints;
		for (int xx = 1; xx < imagenames.size(); xx++) {
			orgImg.load(imagenames[xx - 1]);
			dstImg.load(imagenames[xx]);
//			if (masknames.size() == imagenames.size()) {
//				orgMask.load(masknames[x - 1]);
//				dstMask.load(masknames[x]);
//			}
//			else {
				orgMask = QImage();
				dstMask = QImage();
//			}
			
			TwoViewStereo stereo(views[xx-1], orgImg, orgMask, views[xx], dstImg, dstMask, 0, 255, 256, 0.1);

			stereo.limg = cv::Mat(orgImg.height(), orgImg.width(), CV_8UC4, (uchar*)orgImg.bits(), orgImg.bytesPerLine());
			stereo.rimg = cv::Mat(dstImg.height(), dstImg.width(), CV_8UC4, (uchar*)dstImg.bits(), dstImg.bytesPerLine());
			stereo.computeDepthMaps();

			for (int yy = 0; yy < 1; yy++) {
				Eigen::Vector3d tmppoint = views[xx - 1].fromLocalToGlobal(stereo.threeDpoints[yy].first);
				tmppoint = views[0].fromGlobalToLocal(tmppoint);
				totalPoints.push_back(PLYPoint(tmppoint, stereo.threeDpoints[yy].second));
			}

			QDateTime time = QDateTime::currentDateTime();
			QString timename = time.toString("dd_hh_mm_ss");
			stereo.rightDepthMap().save(DATA_PATH + timename + "_" + QString::number(xx) + ".jpg");
		}
		string plypath = DATA_PATH + string("3dpoint.ply");
		outputPLYFile(plypath, totalPoints);

		cout << "Finished!" << endl;
	}
		break;
	case 4: //without fusion
	{
		mainTrack();
		vector<Eigen::Matrix3d> R; //rotation of vision, load from files
		vector<Eigen::Vector3d> X, Y; //translate vector of imu and vision
		int no = readVisionResult(R, Y);
		/*for (int ii = 0; ii < R.size(); ii++) {
			QString calrtname = QString(DATA_PATH) + "calrt/" + QString("%1").arg(ii, 4, 10, QChar('0')) + ".calrt";
			ofstream ofs(calrtname.toStdString());
			ofs << i << endl;
			for (int x = 0; x < 4; x++) {
				ofs << "0" << endl;
			} //distort
			ofs << max(image.at(0).rows, image.at(0).cols) << endl << "0" << endl << image.at(0).rows / 2 << endl << "0" << endl << max(image.at(0).rows, image.at(0).cols) << endl << image.at(0).cols / 2 << endl;
			ofs << "0" << endl << "0" << endl << "1" << endl; //K
			for (int y = 0; y < 3; y++) {
				ofs << Y.at(i)(y) << endl;
			}
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					ofs << R.at(ii)(x, y) << endl;
				}
			}
			ofs.close();
		}*/

//		mainReconstruct2();
	}	
		break;
	default:
		break;
	}

}

int main() {
//	fusion();
	DATA_PATH = "F:/mycode/TrackAndReconstruct/Data/";
//	DATA_PATH = "F:/mycode/meso/Data/";
//	test(3);
	
	
	string videoname = "F:/mycode/TrackAndReconstruct/Data/sensor3.mp4";
	cv::VideoCapture capture;
	capture.open(videoname);
	if (!capture.isOpened()) {
		cout << "Could not open video" << videoname << "with opencv..." << endl;
		return false;
	}

	cv::Mat frame;

	int no = 0;
	int i = 0;
	vector<cv::Mat> testimages;
	for (;;) {
		capture >> frame;
		if (frame.empty())
			break; 
		if (no % 10 == 0) {
			image.push_back(frame.clone());

			QString picname = QString(DATA_PATH) + "pic/" + QString("%1").arg(i, 4, 10, QChar('0')) + ".jpg";
			cv::imwrite(picname.toStdString(), frame);

			i++;
		}
		no++;

		
	}
	/*
	cout << "Total frams: " << no << " will be added to calculate the camera parameters." << endl;

	vector<Eigen::Matrix3d> R;
	vector<Eigen::Vector3d> t;
	for (int j = 0; j < i; j++) {
		R.push_back(Eigen::Matrix3d::Zero());
		t.push_back(Eigen::Vector3d::Zero());
	}

	ifstream fs(DATA_PATH + string("visionparam.txt"));
	string str;
	int tmp = 0;
	char buf[100];

	while (fs) {
		fs >> buf;
		//sscanf(str.c_str(), "Frame %d", &tmp);
		fs >> tmp;

		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				fs >> R.at(tmp)(x, y);
			}
		}
		for (int y = 0; y < 3; y++) {
			fs >> t.at(tmp)(y);
		}

		no++;

		if (true) {
			QString calrtname = QString(DATA_PATH) + "calrt/" + QString("%1").arg(tmp, 4, 10, QChar('0')) + ".calrt";
			ofstream ofs(calrtname.toStdString());
			ofs << tmp << endl;
			for (int x = 0; x < 4; x++) {
				ofs << "0" << endl;
			} //distort
			ofs << max(image.at(0).rows, image.at(0).cols) << endl << "0" << endl << image.at(0).rows / 2 << endl << "0" << endl << max(image.at(0).rows, image.at(0).cols) << endl << image.at(0).cols / 2 << endl;
			ofs << "0" << endl << "0" << endl << "1" << endl; //K
			for (int y = 0; y < 3; y++) {
				ofs << t.at(tmp)(y) << endl;
			}
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					ofs << R.at(tmp)(x, y) << endl;
				}
			}
			ofs.close();
		}
	}

	fs.close();
	*/
	return 0;
}