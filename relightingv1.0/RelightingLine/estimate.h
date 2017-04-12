#ifndef ESTIMATE_H
#define ESTIMATE_H

#include "ply.h"

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/console/print.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/ply_io.h>
#include <pcl/PolygonMesh.h>
#include <pcl/common/io.h>
#include <pcl/io/io.h>

#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>

#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/keyboard_event.h>

#include <boost/lexical_cast.hpp>

#include <pcl/console/print.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

class Estimation {
public:
	Estimation(string plyFileName, string imgname, string maskname);
	~Estimation() {};

	Eigen::Vector3d estimateLight();
	void calParams(Eigen::Vector3d lightdir, double ts, double th);
	void calNormal(string plyFileName);

	vector<Point> scenePoint;
	pcl::NormalEstimation<pcl::PointXYZRGB, pcl::PointNormal> ne;
	pcl::PointCloud<pcl::PointNormal>::Ptr normals;
	double calAngle(Eigen::Vector3d, Eigen::Vector3d);
	
private:

	bool test();

	string imgname, maskname;
};

#endif