#ifndef XMLREADER_H
#define XMLRENDER_H

#include <vector>
#include <iostream>
using namespace std;

#include <boost/property_tree/xml_parser.hpp>
#include <boost/math/quaternion.hpp>

static const string str_node_name[] = { "name", "frames", "frame-count", "value", "index", "time", "w", "x", "y", "z" };
static const string image_node_name[] = {"sensor-name", "pics.pic","pic-count", "name", "timestamp"};
//sensor xml data sturcture
struct sensorData //sensorData, include acc, gyr, rot, mag
{
	int index;
	double time;
	boost::math::quaternion<double> quat;
};

struct sensorImageData //include image informations
{
	string pic_name;
	double time;
	string sensor_name;
	int pic_count;

	boost::math::quaternion<double> acc;
	boost::math::quaternion<double> rot;
	boost::math::quaternion<double> mag;
	boost::math::quaternion<double> gyr;
};
//complete line, for image sequences
int readTotalXmlFile(std::vector<sensorImageData> &sensorImageDetail, string filename)
{
	sensorImageDetail.clear();

	sensorImageData xml_stage_detail;
	boost::property_tree::ptree pt, root;
	boost::property_tree::ptree subpt;
	try {
		read_xml(filename, pt);

		root = pt.get_child("collection-seq.collections");
	}
	catch (std::exception& e) {
		return -1;
	}
	//root-> collection
	for (boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); it++) {
		boost::property_tree::ptree strage = it->second;
		//sensor-name
		pt = strage.get_child(image_node_name[0]);
		xml_stage_detail.sensor_name = pt.data();

		//pt-> pics
		pt = strage.get_child(image_node_name[1]);
		for (boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); it++) {
			subpt = pt.get_child(image_node_name[3]);
			xml_stage_detail.pic_name = subpt.data();

			subpt = pt.get_child(image_node_name[4]);
			xml_stage_detail.time = atof(subpt.data().c_str());
		}
		
		//pic-count
		pt = strage.get_child(image_node_name[2]);
		xml_stage_detail.pic_count = atoi(pt.data().c_str());

		sensorImageDetail.push_back(xml_stage_detail);
	}
}
//single xml file, for image or video
int readSensorXMLFile(std::vector<sensorData> &sensorAccDetail, std::vector<sensorData> &sensorGyrDetail, std::vector<sensorData> &sensorMagDetail, std::vector<sensorData> &sensorRotDetail, string filename)
{
	sensorAccDetail.clear();
	sensorGyrDetail.clear();
	sensorMagDetail.clear();
	sensorRotDetail.clear();

	boost::property_tree::ptree pt, root;

	try {
		read_xml(filename, pt);
		root = pt.get_child("session.threads.thread.channels");
	}
	catch (std::exception& e) {
		return -1;
	}
	//root-> channel
	for (boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); it++) {

		boost::property_tree::ptree strage = it->second;
		string str_value;
		double qu[4];
		pt = strage.get_child(str_node_name[0]); //pt-> name
		str_value = pt.data();

		//acc
		if (strcmp(str_value.c_str(), "a") == 0 || strcmp(str_value.c_str(), "r") == 0 || strcmp(str_value.c_str(), "g") || strcmp(str_value.c_str(), "m")) {

			try {
				pt = strage.get_child(str_node_name[1]); //pt-> frames

				//for each frame in frames
				for (boost::property_tree::ptree::iterator itr = pt.begin(); itr != pt.end(); itr++) {
					sensorData xml_stage_detail;

					strage = itr->second;

					boost::property_tree::ptree subpt, subsubpt;

					for (int i = 0; i < 3; i++) {
						try {
							subpt = strage.get_child(str_node_name[i + 3]);
						}
						catch (std::exception& e) {
							return -1;
						}

						switch (i)
						{
						case 0: //pt-> value
							try {
								for (int j = 0; j < 4; j++) {
									subsubpt = subpt.get_child(str_node_name[j + 6]);
									qu[j] = atof(subsubpt.data().c_str());
								}
								//w,x,y,z
								xml_stage_detail.quat = boost::math::quaternion<double>(qu[0], qu[1], qu[2], qu[3]);
							}
							catch (std::exception& e) {
								return -1;
							}

							break;
						case 1: //index
							xml_stage_detail.index = atoi(subpt.data().c_str());
							break;
						case 2: //time
							xml_stage_detail.time = atof(subpt.data().c_str());
							break;
						default:
							break;
						}
					}

					if (strcmp(str_value.c_str(), "a") == 0)
						sensorAccDetail.push_back(xml_stage_detail);
					else if (strcmp(str_value.c_str(), "r") == 0)
						sensorRotDetail.push_back(xml_stage_detail);
					else if (strcmp(str_value.c_str(), "m") == 0)
						sensorMagDetail.push_back(xml_stage_detail);
					else 
						sensorGyrDetail.push_back(xml_stage_detail);
				}
			}
			catch (std::exception& e) {
				return -1;
			}
		}
	}

	return 0;
}
//align the time stamp, for image sequences
int TimeAlignment(std::vector<sensorImageData> &sensorImageDetail, std::vector<sensorData> sensorAccDetail, std::vector<sensorData> sensorRotDetail, std::vector<sensorData> sensorMagDetail, std::vector<sensorData> sensorGyrDetail)
{
	double imgTime, accTime, rotTime, magTime, gyrTime;
	double timeDiff = 1.0e9;
	int no_acc = 0, no_rot = 0, no_mag = 0, no_gyr = 0;
	int idx_acc = 0, idx_rot = 0, idx_mag = 0, idx_gyr = 0;


	for (int i = 0; i < sensorImageDetail.size(); i++) {
		imgTime = sensorImageDetail[i].time;
		timeDiff = 1.0e9;
		for (idx_acc = no_acc; idx_acc < sensorAccDetail.size(); idx_acc++) {
			accTime = sensorAccDetail[i].time;

			if (timeDiff > fabs(accTime - imgTime)) {
				no_acc = idx_acc;
				timeDiff = fabs(accTime - imgTime);
			}

			if (imgTime < accTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_rot = no_rot; idx_rot < sensorRotDetail.size(); idx_rot++) {
			rotTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(rotTime - imgTime)) {
				no_rot = idx_rot;
				timeDiff = fabs(rotTime - imgTime);
			}

			if (imgTime < rotTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_mag = no_mag; idx_mag < sensorRotDetail.size(); idx_mag++) {
			magTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(magTime - imgTime)) {
				no_mag = idx_mag;
				timeDiff = fabs(magTime - imgTime);
			}

			if (imgTime < magTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_gyr = no_gyr; idx_gyr < sensorRotDetail.size(); idx_gyr++) {
			rotTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(rotTime - imgTime)) {
				no_gyr = idx_gyr;
				timeDiff = fabs(rotTime - imgTime);
			}

			if (imgTime < rotTime) break;
		}

		sensorImageDetail[i].acc = sensorAccDetail[no_acc].quat;
		sensorImageDetail[i].rot = sensorRotDetail[no_rot].quat;
		sensorImageDetail[i].mag = sensorAccDetail[no_mag].quat;
		sensorImageDetail[i].gyr = sensorRotDetail[no_gyr].quat;
	}

	assert(sensorAccDetail.size() == sensorRotDetail.size() && sensorAccDetail.size() == sensorMagDetail.size() && sensorAccDetail.size() == sensorGyrDetail.size());

	return 0;
}

//align the time stamp, for video
int TimeAlignmentVideo(std::vector<sensorImageData> &sensorImageDetail, std::vector<sensorData> sensorAccDetail, std::vector<sensorData> sensorGyrDetail, std::vector<sensorData> sensorMagDetail, std::vector<sensorData> sensorRotDetail) {
	int numOfFrame = sensorImageDetail.size();

	double mintime = sensorAccDetail[0].time, maxtime = sensorAccDetail.back().time;
	if (sensorGyrDetail[0].time < mintime) mintime = sensorGyrDetail[0].time;
	if (sensorRotDetail[0].time < mintime) mintime = sensorRotDetail[0].time;
	if (sensorMagDetail[0].time < mintime) mintime = sensorMagDetail[0].time;
	if (sensorGyrDetail.back().time > maxtime) maxtime = sensorGyrDetail.back().time;
	if (sensorRotDetail.back().time > maxtime) maxtime = sensorRotDetail.back().time;
	if (sensorMagDetail.back().time > maxtime) maxtime = sensorMagDetail.back().time;

	//begin
	double accTime, rotTime, magTime, gyrTime;
	double imgTime = mintime, timeDiff = 1.0e9;
	int no_acc = 0, no_rot = 0, no_mag = 0, no_gyr = 0;
	int idx_acc = 0, idx_rot = 0, idx_mag = 0, idx_gyr = 0;
	double step = (maxtime - mintime) / numOfFrame;
	for (int i = 0; i < numOfFrame; i++) {
		timeDiff = 1.0e9;
		for (idx_acc = no_acc; idx_acc < sensorAccDetail.size(); idx_acc++) {
			accTime = sensorAccDetail[i].time;

			if (timeDiff > fabs(accTime - imgTime)) {
				no_acc = idx_acc;
				timeDiff = fabs(accTime - imgTime);
			}

			if (imgTime < accTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_rot = no_rot; idx_rot < sensorRotDetail.size(); idx_rot++) {
			rotTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(rotTime - imgTime)) {
				no_rot = idx_rot;
				timeDiff = fabs(rotTime - imgTime);
			}

			if (imgTime < rotTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_mag = no_mag; idx_mag < sensorRotDetail.size(); idx_mag++) {
			magTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(magTime - imgTime)) {
				no_mag = idx_mag;
				timeDiff = fabs(magTime - imgTime);
			}

			if (imgTime < magTime) break;
		}
		timeDiff = 1.0e9;
		for (idx_gyr = no_gyr; idx_gyr < sensorRotDetail.size(); idx_gyr++) {
			rotTime = sensorRotDetail[i].time;

			if (timeDiff > fabs(rotTime - imgTime)) {
				no_gyr = idx_gyr;
				timeDiff = fabs(rotTime - imgTime);
			}

			if (imgTime < rotTime) break;
		}

		sensorImageDetail[i].acc = sensorAccDetail[no_acc].quat;
		sensorImageDetail[i].rot = sensorRotDetail[no_rot].quat;
		sensorImageDetail[i].mag = sensorAccDetail[no_mag].quat;
		sensorImageDetail[i].gyr = sensorRotDetail[no_gyr].quat;
		sensorImageDetail[i].time = imgTime;

		imgTime += step;
	}

	return 0;
}
#endif