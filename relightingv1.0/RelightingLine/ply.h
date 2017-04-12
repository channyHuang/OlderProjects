#ifndef PLY_H
#define PLY_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include <Eigen/Eigen>

typedef Eigen::Vector3d Point3d;
typedef Eigen::Vector2d Point2d;
typedef pair<Point3d, Point3d> Point;

static char ply_header_normal[] =
"ply\n"
"format ascii 1.0\n"
"element face 0\n"
"property list uchar int vertex_indices\n"
"element vertex %d\n"
"property float x\n"
"property float y\n"
"property float z\n"
"property float nx\n"
"property float ny\n"
"property float nz\n"
"end_header\n";

static void DumpPointsToPly(vector<Point> pointCloud, string plyFileName) {
	ofstream fout(plyFileName.c_str());
	if (!fout) {
		cout << "Ply file open failed..." << endl;
		return;
	}
	fout << ply_header_normal;
	for (int i = 0; i < pointCloud.size(); i++) {
		fout << pointCloud[i].first[0] << " " << pointCloud[i].first[1] << " " << pointCloud[i].first[2] << " " << pointCloud[i].second[0] << " " << pointCloud[i].second[1] << " " << pointCloud[i].second[2] << endl;
	}
	fout.close();
}

static void DumpPlyToPoints(vector<Point> &pointCloud, string plyFileName) {
	//ply file contains point, color and normal
	ifstream fin(plyFileName.c_str());
	if (!fin) {
		cout << "Ply file open failed..." << endl;
		exit(0);
	}
	string line;
	for (int i = 0; i < 13; i++) getline(fin, line);
	Point p;
	while (fin) {
		fin >> p.first[0] >> p.first[1] >> p.first[2] >> p.second[0] >> p.second[1] >> p.second[2] >> p.second[0] >> p.second[1] >> p.second[2];
		pointCloud.push_back(p);
	}
	fin.close();

	cout << "Point number: " << pointCloud.size() << endl;
}

#endif