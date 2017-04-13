#include "fusion.h"

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QtCore/QString>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

namespace TrackSensor {
	bool Fusion::init(std::string xmlName, std::string videoName, int frameNum) {
		xmlNameStr = xmlName;
		videoNameStr = videoName;
		//write image sequence
		cv::VideoCapture capture;
		capture.open(videoNameStr);
		if (!capture.isOpened()) {
			std::cout << "Could not open video" << videoNameStr << "with opencv..." << std::endl;
			return false;
		}
		cv::Mat frame;
		int no = 0, totalFrameNum = capture.get(CV_CAP_PROP_FRAME_COUNT), frameStep = totalFrameNum / frameNum;
		int i = 0;
		for (;;) {
			capture >> frame;
			if (frame.empty())
				break;
			if (no % frameStep == 0) {
				//image.push_back(frame.clone());
				QString picname = QString(DATA_PATH.c_str()) + "/pic/" + QString("%1").arg(i, 8, 10, QChar('0')) + ".jpg";
				cv::imwrite(picname.toStdString(), frame);
				++i;
			}
			if (i >= frameNum) break;
			no++;
		}

		if (!sensordata.load(xmlNameStr)) return false;

		auto node = sensordata.get_node("0");
		auto acce = *node.channel("a"), gyre = *node.channel("g"), rote = *node.channel("r"), mage = *node.channel("m");

		totalFrame = std::min(std::min(std::min(acce.frame_count(), gyre.frame_count()), rote.frame_count()), mage.frame_count());

		for (int i = 0; i < acce.frame_count(); i++) {
			accData.push_back(acce.frame(i));
		}
		for (int i = 0; i < gyre.frame_count(); i++) {
			gyrData.push_back(gyre.frame(i));
		}
		for (int i = 0; i < rote.frame_count(); i++) {
			rotData.push_back(rote.frame(i));
		}
		for (int i = 0; i < mage.frame_count(); i++) {
			magData.push_back(mage.frame(i));
		}

		numOfFrame = frameNum;
//		sRc = Eigen::Matrix3d::Identity(3, 3);

		sensorFilter.init();

		return true;
	}
	
	capg::sensor::Frame Fusion::BSpine(capg::sensor::Frame a, capg::sensor::Frame b, capg::sensor::Frame c, capg::sensor::Frame d, double t, int index) {
		capg::sensor::Frame p;
		double a1, a2, a3, a4;
		/*a1 = (1.0 - t)*(1.0 - t)*(1.0 - t) / 6.0;
		a2 = (3.0 * t*t*t - 6.0 * t*t + 4.0) / 6.0f;
		a3 = (-3.0 * t*t*t + 3.0 * t*t + 3.0 * t + 1.0) / 6.0f;
		a4 = t*t*t / 6.0;
		p.x = a1*a.x + a2*b.x + a3*c.x + a4*d.x;
		p.y = a1*a.y + a2*b.y + a3*c.y + a4*d.y;
		p.z = a1*a.z + a2*b.z + a3*c.z + a4*d.z;
		p.w = a1*a.w + a2*b.w + a3*c.w + a4*d.w;*/
		p.x = (a.x + b.x + c.x + d.x) / 4.0;
		p.y = (a.y + b.y + c.y + d.y) / 4.0;
		p.z = (a.z + b.z + c.z + d.z) / 4.0;
		p.w = (a.w + b.w + c.w + d.w) / 4.0;

		p.index = index;
		p.time = t;

		return p;
	}

	double Fusion::TimeAlignment() {
		if (accData.size() < 5 || gyrData.size() < 5 || rotData.size() < 5 || magData.size() < 5) {
			std::cout << "Error in Fusion::TimeAlignment, xml data size is too small...";
			exit(0);
		}

		double mintime = accData[3].time, maxtime = accData.back().time;
		if (gyrData[3].time > mintime) mintime = gyrData[3].time;
		if (rotData[3].time > mintime) mintime = rotData[3].time;
		if (magData[3].time > mintime) mintime = magData[3].time;
		if (gyrData.back().time < maxtime) maxtime = gyrData.back().time;
		if (rotData.back().time < maxtime) maxtime = rotData.back().time;
		if (magData.back().time < maxtime) maxtime = magData.back().time;

		//begin
		double timeDiff = mintime;
		int noacc = 3, nogyr = 3, norot = 3, nomag = 3;
		//align time
#if 1
		int index = 0;
		int framestep = totalFrame / numOfFrame;
		Eigen::Vector3d CurrentVelocity = Eigen::Vector3d(0, 0, 0);
		double oldTimeDiff = mintime;

		std::ofstream ofs("fusion.txt");

		while (timeDiff < maxtime) {
			imageFrames imgFrame;
			index++;

			while ((accData[noacc].time <= timeDiff) && (noacc < accData.size() - 1)) {
				noacc++;
			}
			imgFrame.accFrame = BSpine(accData[noacc - 3], accData[noacc - 2], accData[noacc - 1], accData[noacc], timeDiff, index);
			while ((gyrData[nogyr].time <= timeDiff) && (nogyr < gyrData.size() - 1)) {
				nogyr++;
			}
			imgFrame.gyrFrame = BSpine(gyrData[nogyr - 3], gyrData[nogyr - 2], gyrData[nogyr - 1], gyrData[nogyr], timeDiff, index);
			while ((rotData[norot].time <= timeDiff) && (norot < rotData.size() - 1)) {
				norot++;
			}
			imgFrame.rotFrame = BSpine(rotData[norot - 3], rotData[norot - 2], rotData[norot - 1], rotData[norot], timeDiff, index);
			while ((magData[nomag].time <= timeDiff) && (nomag < magData.size() - 1)) {
				nomag++;
			}

			imgFrame.magFrame = BSpine(magData[nomag - 3], magData[nomag - 2], magData[nomag - 1], magData[nomag], timeDiff, index);

			imgFrame.timeStep = timeDiff - oldTimeDiff;
			imgFrame.velocity = CurrentVelocity + Eigen::Vector3d(imgFrame.accFrame.x, imgFrame.accFrame.y, imgFrame.accFrame.z)*imgFrame.timeStep;
			
			filtering(imgFrame);

			if (index % framestep == 0) {
				imgFrames.push_back(imgFrame);

				//ofs << imgFrame.rotFrame.w << " " << imgFrame.rotFrame.x << " " << imgFrame.rotFrame.y << " " << imgFrame.rotFrame.z << std::endl;

				if (false){//output 

					//rotation vector
					Eigen::Matrix3d mout;
					quat2mat(mout, imgFrame.rotFrame);

					for (int xx = 0; xx < 3; xx++) {
						for (int yy = 0; yy < 3; yy++) {
							ofs << mout(xx, yy) << " ";
						}
						ofs << std::endl;
					}
					ofs << (imgFrame.velocity(0) + imgFrame.accFrame.x * imgFrame.timeStep / 2.0) * imgFrame.timeStep << " " << (imgFrame.velocity(1) + imgFrame.accFrame.y * imgFrame.timeStep / 2.0) * imgFrame.timeStep << " " << (imgFrame.velocity(2) + imgFrame.accFrame.z * imgFrame.timeStep / 2.0) * imgFrame.timeStep << std::endl;
				}

				if (imgFrames.size() == numOfFrame) break;
			}

			oldTimeDiff = timeDiff;
			timeDiff = std::min(std::min(std::min(accData[noacc].time, gyrData[nogyr].time), magData[nomag].time), rotData[norot].time);
			CurrentVelocity = imgFrame.velocity;
		}
		ofs.close();
#endif // 1

		//align images
		double timestep;
		timestep = (maxtime - mintime) / numOfFrame;
#if 0
		index = 0;
		int indexStep = tmpImgFrames.size() / numOfFrame;
		for (int i = 0; i < numOfFrame; i++) {
			imgFrames.push_back(tmpImgFrames.at(index));
			index += indexStep;
		}
#endif
#if 0		
		for (int i = 0; i < numOfFrame; i++) {
			timeDiff += timestep;
			while (accData[noacc].time < timeDiff && noacc < accData.size() - 1) noacc++;
			imgFrame.accFrame = BSpine(accData[noacc - 3], accData[noacc - 2], accData[noacc - 1], accData[noacc], timeDiff, i);
			while (gyrData[nogyr].time < timeDiff && nogyr < gyrData.size() - 1) nogyr++;
			imgFrame.gyrFrame = BSpine(gyrData[nogyr - 3], gyrData[nogyr - 2], gyrData[nogyr - 1], gyrData[nogyr], timeDiff, i);
			while (rotData[norot].time < timeDiff && norot < rotData.size() - 1) norot++;
			imgFrame.rotFrame = BSpine(rotData[norot - 3], rotData[norot - 2], rotData[norot - 1], rotData[norot], timeDiff, i);
			while (magData[nomag].time < timeDiff && nomag < magData.size() - 1) nomag++;
			imgFrame.magFrame = BSpine(magData[nomag - 3], magData[nomag - 2], magData[nomag - 1], magData[nomag], timeDiff, i);

			imgFrames.push_back(imgFrame);

		}
#endif // 1
		return timestep;
	}

	void Fusion::filtering(imageFrames &Frame)	{
//		for (int i = 0; i < numOfFrame; i++) {
			sensorFilter.MahonyAHRSupdate(Frame.rotFrame.x, Frame.rotFrame.y, Frame.rotFrame.z, Frame.accFrame.x, Frame.accFrame.y, Frame.accFrame.z, Frame.magFrame.x, Frame.magFrame.y, Frame.magFrame.z);

			Frame.rotFrame.w = sensorFilter.q0;
			Frame.rotFrame.x = sensorFilter.q1;
			Frame.rotFrame.y = sensorFilter.q2;
			Frame.rotFrame.z = sensorFilter.q3;
//		}

	}

	void Fusion::test(std::string xmlname, std::string videoname, int frameNum) {
		if (!init(xmlname, videoname, frameNum)) {
			std::cout << "Load xml file failed...";
			exit(0);
		}
		TimeAlignment();		

		//write camera params
		outputRT(imgFrames);
		
	}

	void Fusion::outputRT(std::vector<imageFrames> Frames) {
		for (int i = 0; i < Frames.size(); i++) {
			QString picname = QString(DATA_PATH.c_str()) + "/calrt/" + QString("%1").arg(i, 8, 10, QChar('0')) + ".txt";
			//rotation vector
			Eigen::Matrix3d mout;
			quat2mat(mout, Frames.at(i).rotFrame);

			std::ofstream ofs(picname.toStdString());
			for (int xx = 0; xx < 3; xx++) {
				for (int yy = 0; yy < 3; yy++) {
					ofs << mout(xx, yy) << " ";
				}
				ofs << std::endl;
			}
			Eigen::Vector3d translateVec = (Frames.at(i).velocity + Eigen::Vector3d(Frames.at(i).accFrame.x, Frames.at(i).accFrame.y, Frames.at(i).accFrame.z)*Frames.at(i).timeStep / 2.0) * Frames.at(i).timeStep; //s = v0 * t + 1/2 * a * t^2;
			ofs << translateVec(0) << " " << translateVec(1) << " " << translateVec(2) << std::endl;
			ofs.close();
		}
	}

	void Fusion::euler2mat(Eigen::Matrix3d &mout, capg::sensor::Frame in) {
		Eigen::AngleAxisd ang(1, Eigen::Vector3d(in.x, in.y, in.z));
		mout = ang.toRotationMatrix();
	}

	void Fusion::quat2mat(Eigen::Matrix3d &mout, capg::sensor::Frame in)
	{	
		Eigen::Quaternion<double> qin = Eigen::Quaternion<double>(in.w, in.x, in.y, in.z);
		/*Eigen::Matrix3d */mout = qin.toRotationMatrix();
		//Eigen::Vector3d vout = qin.vec();
	}
}