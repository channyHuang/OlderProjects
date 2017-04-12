#include "estimate.h"

#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <iostream>
using namespace std;

#include "vectorimage.hpp"

#include <windows.h>
#include <assert.h>

#define Pi 3.1415926
#undef max
#undef min

bool
loadCloud(const std::string &filename, pcl::PointCloud<pcl::PointXYZRGB> cloud)
{
	TicToc tt;
	print_highlight("Loading "); print_value("%s ", filename.c_str());

	pcl::PLYReader reader;
	tt.tic();
	if (reader.read(filename, cloud) < 0)
		return (false);
	print_info("[done, "); print_value("%g", tt.toc()); print_info(" ms : "); print_value("%d", cloud.width * cloud.height); print_info(" points]\n");
	print_info("Available dimensions: "); print_value("%s\n", pcl::getFieldsList(cloud).c_str());

	return (true);
}

void
saveCloud(const std::string &filename, const pcl::PointCloud<pcl::PointXYZRGB> &cloud, bool format)
{
	TicToc tt;
	tt.tic();

	print_highlight("Saving "); print_value("%s ", filename.c_str());

	pcl::PCDWriter writer;
	writer.write(filename, cloud, format);

	//print_info("[done, "); print_value("%g", tt.toc()); print_info(" ms : "); print_value("%d", cloud.width * cloud.height); print_info(" points]\n");
}



Estimation::Estimation(string plyFileName, string imgname, string maskname) {
	this->imgname = imgname;
	this->maskname = maskname;

	//change .obj to .ply
	int lenPly = plyFileName.length();
	if (plyFileName[lenPly - 1] == 'j') {
		cout << "Read obj file..." << endl;
		pcl::PolygonMesh mesh;
		pcl::io::load(plyFileName, mesh);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::fromPCLPointCloud2(mesh.cloud, *cloud);
		//pcl::io::savePCDFileASCII(plyFileName + ".pcd", *cloud);
		//cout << cloud->size() << endl;
		scenePoint.clear();
		for (int i = 0; i < cloud->size(); i++) {
			Point p = pair<Point3d, Point3d>(Point3d(cloud->at(i).x, cloud->at(i).y, cloud->at(i).z), Point3d(0, 0, 0));
			scenePoint.push_back(p);
		}
	}
	else {
		cout << "Read ply file..." << endl;
		DumpPlyToPoints(scenePoint, plyFileName);
	}

	calNormal(plyFileName);
	cout << "Scene point number: " << scenePoint.size() << endl;
	cout << "Finish reading..." << endl;
}
//just estimate direction // not and location
Eigen::Vector3d Estimation::estimateLight()
{
	cout << "Begin to estimate..." << endl;
	//read image and save value I
	VectorImage imgdata, maskdata;
	imgdata = imgdata.fromFile(imgname);
	maskdata = maskdata.fromFile(maskname);

	if (imgdata.isNull()) {
		QMessageBox::critical(NULL, "Error", "Image is empty, please check the path and try again...");
		return Eigen::Vector3d(0, 0, 0);
	}

	if (maskdata.isNull()) {
		QImage mask(imgdata.width(), imgdata.height(), QImage::Format_RGB32);
		mask.createMaskFromColor(RGB_MASK);
		maskdata.fromQImage(mask);
		cout << "Have not found the mask image. The whole image will be used in calculate." << endl;
	}

	int numPixel = imgdata.width() * imgdata.height();
	vector<RGBA> valueI;
	for (size_t x = 0; x < imgdata.width(); x++) {
		for (size_t y = 0; y < imgdata.height(); y++) {
			if (maskdata.pixel(x, y) != WHITE) 	continue;
			RGBA vI = imgdata.pixel(x, y);
			valueI.push_back(vI);
		}
	}
	cout << valueI.size() << " " << scenePoint.size() << endl;
	//assert(valueI.size() == scenePoint.size());

	time_t startTime = GetTickCount();

	Eigen::Vector3d lpos = Eigen::Vector3d::Zero();

	size_t n = 500;
	size_t times = scenePoint.size() / n;

	for (size_t i = 0; i < times; i++) {
		Eigen::MatrixX3d A(n, 3);
		Eigen::VectorXd b(n);
		{
			for (size_t j = 0; j < n; j++) {
				size_t index = i*n + j;

				if (index >= valueI.size() || index >= scenePoint.size()) {
					break;
				}

				A(j, 0) = normals->at(index).normal_x;
				A(j, 1) = normals->at(index).normal_y;
				A(j, 2) = normals->at(index).normal_z;

				//double r = 0.2126*valueI[index].r + 0.7152*valueI[index].g + 0.0722*valueI[index].b;
				//double r = 0.29900*valueI[index].r + 0.58700*valueI[index].g + 0.11400*valueI[index].b;
				double r = valueI[index].r;
				b(j) = r + normals->at(index).normal_x * scenePoint[index].first[0] + normals->at(index).normal_y * scenePoint[index].first[1] + normals->at(index).normal_z * scenePoint[index].first[2];
			}

			Eigen::VectorXd L = A.colPivHouseholderQr().solve(b);
			if (!isnan(L(0)) && !isnan(L(1)) && !isnan(L(2))) {
				L.normalize();

				lpos += L;
			}
		}
		{
			//g and b
			for (size_t j = 0; j < n; j++) {
				size_t index = i*n + j;

				if (index >= valueI.size()) {
					break;
				}

				A(j, 0) = normals->at(index).normal_x;
				A(j, 1) = normals->at(index).normal_y;
				A(j, 2) = normals->at(index).normal_z;

				//double r = 0.2126*valueI[index].r + 0.7152*valueI[index].g + 0.0722*valueI[index].b;
				//double r = 0.29900*valueI[index].r + 0.58700*valueI[index].g + 0.11400*valueI[index].b;
				double r = valueI[index].g;
				b(j) = r + normals->at(index).normal_x * scenePoint[index].first[0] + normals->at(index).normal_y * scenePoint[index].first[1] + normals->at(index).normal_z * scenePoint[index].first[2];
			}

			Eigen::VectorXd L = A.colPivHouseholderQr().solve(b);
			if (!isnan(L(0)) && !isnan(L(1)) && !isnan(L(2))) {
				L.normalize();

				lpos += L;
			}
		}
		{
			for (size_t j = 0; j < n; j++) {
				size_t index = i*n + j;

				if (index >= valueI.size()) {
					break;
				}

				A(j, 0) = normals->at(index).normal_x;
				A(j, 1) = normals->at(index).normal_y;
				A(j, 2) = normals->at(index).normal_z;

				//double r = 0.2126*valueI[index].r + 0.7152*valueI[index].g + 0.0722*valueI[index].b;
				//double r = 0.29900*valueI[index].r + 0.58700*valueI[index].g + 0.11400*valueI[index].b;
				double r = valueI[index].b;
				b(j) = r + normals->at(index).normal_x * scenePoint[index].first[0] + normals->at(index).normal_y * scenePoint[index].first[1] + normals->at(index).normal_z * scenePoint[index].first[2];
			}

			Eigen::VectorXd L = A.colPivHouseholderQr().solve(b);
			if (!isnan(L(0)) && !isnan(L(1)) && !isnan(L(2))) {
				L.normalize();

				lpos += L;
			}
		}
	}

	lpos.normalize();

	time_t endTime = GetTickCount();

	cout << "End: light position is: " << lpos[0] << " " << lpos[1] << " " << lpos[2] << endl;
	cout << "Time used: " << endTime - startTime << endl;

	return lpos;
}

void Estimation::calParams(Eigen::Vector3d lightdir, double ts, double th)
{
//	test();

	cout << "Begin to calculate intermediate params..." << endl;
	time_t startTime = GetTickCount();

	VectorImage imgdata, maskdata;
	imgdata = imgdata.fromFile(imgname);
	maskdata = maskdata.fromFile(maskname);

	if (imgdata.isNull()) {
		QMessageBox::critical(NULL, "Error", "Image is empty, please check the path and try again...");
		return;
	}

	if (maskdata.isNull()) {
		QImage mask(imgdata.width(), imgdata.height(), QImage::Format_RGB32);
		mask.createMaskFromColor(RGB_MASK);
		maskdata.fromQImage(mask);
		cout << "Have not found the mask image. The whole image will be used in calculate." << endl;
	}

	int numPixel = imgdata.width() * imgdata.height();
	
	double sumpixel[3] = {0, 0, 0};
	double sumsita = 0;
	double sumsita2 = 0;
	int index = 0;
	double sumisita[3] = { 0, 0, 0 };
	for (size_t x = 0; x < imgdata.width(); x++) {
		for (size_t y = 0; y < imgdata.height(); y++) {
			if (maskdata.pixel(x, y) == BLACK) 	continue;

			//sigma sita / sita^2
			double tmpangle = calAngle(scenePoint[index].second, lightdir);
			index++;
			if (index >= scenePoint.size()) {
			//	QMessageBox::warning(NULL, "Error", "Index out of range");
				cout << "Index out of range in function calParams..." << endl;
				break;
			}
			sumsita += tmpangle;
			sumsita2 += (tmpangle * tmpangle);

			//sigma I
			sumpixel[0] += imgdata.pixel(x, y).r / 255.0;
			sumpixel[1] += imgdata.pixel(x, y).g / 255.0;
			sumpixel[2] += imgdata.pixel(x, y).b / 255.0;
			
			//sigma I*sita
			sumisita[0] += (imgdata.pixel(x, y).r / 255.0 * tmpangle);
			sumisita[1] += (imgdata.pixel(x, y).g / 255.0* tmpangle);
			sumisita[2] += (imgdata.pixel(x, y).b / 255.0* tmpangle);
		}
		if (index >= scenePoint.size()) {
			break;
		}
	} 
	/*
	cout << "sumpixel: " << sumpixel[0] << " " << sumpixel[1] << " " << sumpixel[2] << endl;
	cout << "sumsita: " << sumsita << " " << sumsita2 << endl;
	cout << "sumisita: " << sumisita[0] << " " << sumisita[1] << " " << sumisita[2] << endl;*/
	cout << "End: and begin to calculate final params..." << endl;

	//A and D
/*	double D = ( (sumisita[0] - sumsita * sumpixel[0] * 1.0 / numPixel) + (sumisita[1] - sumsita * sumpixel[1] * 1.0 / numPixel) + (sumisita[2] - sumsita * sumpixel[2] * 1.0 / numPixel) )/ (sumsita2 - sumsita * sumsita / numPixel);
	double A = ( (sumpixel[0] - D*sumsita)*1.0 + (sumpixel[1] - D*sumsita)*1.0 + (sumpixel[2] - D*sumsita)*1.0 )/ numPixel;

	cout << "D and A: " << D / 3.0 << " " << A / 3.0 << endl;
*/
	double A = (sumisita[0] + sumisita[1] + sumisita[2] - sumsita2 * (sumpixel[0] + sumpixel[1] + sumpixel[2]) / sumsita ) / (sumsita - sumsita2* numPixel/sumsita);
	double D = (sumpixel[0] + sumpixel[1] + sumpixel[2] - numPixel*A*3.0) / sumsita;
	cout << "A and D: " << A / 3.0 << " " << D / 3.0 << endl;

	//shadowed
	//highlight
	VectorImage Sp(imgdata.width(), imgdata.height(), BLACK);
	VectorImage IkHp(imgdata.width(), imgdata.height(), BLACK);
	//small positive thresholds
	//double ts = 50, th = 0.7;
	for (size_t x = 0; x < imgdata.width(); x++) {
		for (size_t y = 0; y < imgdata.height(); y++) {
			if (maskdata.pixel(x, y) == BLACK) 	continue;
			RGBA rgb(0, 0, 0), rgb2(0, 0, 0);
			//r
			if (imgdata.pixel(x, y).r / 255.0 > A + ts*D) {
				rgb.r = 1;
			}
			else if (imgdata.pixel(x, y).r / 255.0 > A) {
				rgb.r = (imgdata.pixel(x, y).r / 255.0 - A) / (ts*D);
			}

			if (imgdata.pixel(x, y).r / 255.0> A + (1 + th)*D) {
				rgb2.r = imgdata.pixel(x, y).r / 255.0 - A - D;
			}
			else if (imgdata.pixel(x, y).r / 255.0 > A + (1 - th)*D) {
				rgb2.r = (imgdata.pixel(x, y).r / 255.0 - A - (1 - th)*D) *(imgdata.pixel(x, y).r / 255.0 - A - (1 - th)*D) / (4 * th*D);
			}
			//g
			if (imgdata.pixel(x, y).g / 255.0 > A + ts*D) {
				rgb.g = 1;
			}
			else if (imgdata.pixel(x, y).g / 255.0 > A) {
				rgb.g = (imgdata.pixel(x, y).g / 255.0 - A) / (ts*D);
			}

			if (imgdata.pixel(x, y).g / 255.0> A + (1 + th)*D) {
				rgb2.g = imgdata.pixel(x, y).g / 255.0 - A - D;
			}
			else if (imgdata.pixel(x, y).g / 255.0 > A + (1 - th)*D) {
				rgb2.g = (imgdata.pixel(x, y).g / 255.0 - A - (1 - th)*D) *(imgdata.pixel(x, y).g / 255.0 - A - (1 - th)*D) / (4 * th*D);
			}
			//b
			if (imgdata.pixel(x, y).b / 255.0 > A + ts*D) {
				rgb.b = 1;
			}
			else if (imgdata.pixel(x, y).b / 255.0 > A) {
				rgb.b = (imgdata.pixel(x, y).b / 255.0 - A) / (ts*D);
			}
			
			if (imgdata.pixel(x, y).b / 255.0 > A + (1 + th)*D) {
				rgb2.b = imgdata.pixel(x, y).b / 255.0 - A - D;
			}
			else if (imgdata.pixel(x, y).b / 255.0> A + (1 - th)*D) {
				rgb2.b = (imgdata.pixel(x, y).b / 255.0 - A - (1 - th)*D) *(imgdata.pixel(x, y).b / 255.0 - A - (1 - th)*D) / (4 * th*D);
			}

			rgb.r *= 255.0;
			rgb.g *= 255.0;
			rgb.b *= 255.0;
			rgb2.r *= 255.0;
			rgb2.g *= 255.0;
			rgb2.b *= 255.0;
			Sp.setPixel(x, y, rgb);
			IkHp.setPixel(x, y, rgb2);
		}		
	}

	//show image
	QImage imgSp, imgIkHp;
	imgSp = Sp.toQImage(Sp);
	imgIkHp = IkHp.toQImage(IkHp);

	if (QFile::exists((imgname + ".Sp.jpg").c_str())) 
		QFile::remove((imgname + ".Sp.jpg").c_str());
	if (QFile::exists((imgname + ".IkHp.jpg").c_str()))
		QFile::remove((imgname + ".IkHp.jpg").c_str());

	imgSp.save((imgname + ".Sp.jpg").c_str());
	imgIkHp.save((imgname + ".IkHp.jpg").c_str());

	time_t endTime = GetTickCount();
	cout << "End. See the output file please." << endl;
	cout << "Time used: " << endTime - startTime << endl;
	
};

double Estimation::calAngle(Eigen::Vector3d a, Eigen::Vector3d b)
{ 
	return acos(a.dot(b) / (a.norm()*b.norm()));
}

bool Estimation::test()
{
	//angle calculate
	Eigen::Vector3d a = { 0, 1, 0 };
	Eigen::Vector3d b = { 0, 1, 0 };
	assert(fabs(calAngle(a, b)) < 1e-7);

	a = { 0, 1, 0 };
	b = { -1, sqrt(3.0), 0 };
	assert(fabs(calAngle(a, b) - Pi / 6.0) < 1e-7);

	a = { 1, 1, 0 };
	b = { 0, 1, 0 };
	assert(fabs(calAngle(a, b) - Pi / 4.0) < 1e-7);

	a = { 1, 0, 0 };
	b = { 1, sqrt(3.0), 0 };
	assert(fabs(calAngle(a, b) - Pi / 3.0) < 1e-7);

	a = { 1, 0, 0 };
	b = { 0, 1, 0 };
	assert(fabs(calAngle(a, b) - Pi / 2.0) < 1e-7);

	a = { -1, 0, 0 };
	b = { 1, 0, 0 };
	assert(fabs(calAngle(a, b) - Pi ) < 1e-7);

	return true;
}

void Estimation::calNormal(string plyFileName)
{
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	//read data
	int lenPly = plyFileName.length();
	if (plyFileName[lenPly - 1] == 'j') {
		cout << "Read obj file..." << endl;
		pcl::PolygonMesh mesh;
		pcl::io::load(plyFileName, mesh);
		pcl::fromPCLPointCloud2(mesh.cloud, *cloud);
		//pcl::io::savePCDFileASCII(plyFileName + ".pcd", *cloud);
		//cout << "Point sizes: " << cloud->size() << endl;

		//pcl::io::loadPCDFile(plyFileName + ".pcd", *cloud);
	}

	else {
		for (int i = 0; i < scenePoint.size(); i++) {
			pcl::PointXYZRGB p(scenePoint[i].second[0], scenePoint[i].second[1], scenePoint[i].second[2]);
			p.x = scenePoint[i].first[0];
			p.y = scenePoint[i].first[1];
			p.z = scenePoint[i].first[2];
			cloud->push_back(p);
		}
	}

	cout << "Point cloud number: " << cloud->size() << endl;
	cout << "Begin to calculate normals..." << endl;
	//begin to calculate normals
//	pcl::PointCloud<pcl::PointNormal>::Ptr normals(new pcl::PointCloud<pcl::PointNormal>);
//	pcl::NormalEstimation<pcl::PointXYZRGB, pcl::PointNormal> ne;
	normals = pcl::PointCloud<pcl::PointNormal>::Ptr(new pcl::PointCloud<pcl::PointNormal>);

	ne.setInputCloud(cloud);

	pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>);
	ne.setSearchMethod(tree);
	ne.setRadiusSearch(3);

	ne.compute (*normals);
	cout << "Normal size: " << normals->size() << endl;
//	pcl::PLYWriter writer;
//	writer.write("normal.ply", *normals);
	
	if (false) {
		cout << "Show result..." << endl;
		//visualize 
		//show normals and pointClounds
		boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
		viewer->setBackgroundColor(0, 0, 0);
		pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
		viewer->addPointCloud<pcl::PointXYZRGB>(cloud, rgb, "sample cloud");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");

		viewer->addPointCloudNormals<pcl::PointXYZRGB, pcl::PointNormal>(cloud, normals, 10, 0.05, "normals");
		viewer->initCameraParameters();

		while (!viewer->wasStopped())
		{
			viewer->spinOnce();
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
		}
	}
	
}