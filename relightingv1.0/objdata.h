#ifndef OBJDATA_H
#define OBJDATA_H

#include <QString>
#include <vector>
#include <map>
using namespace std;

#include <QMessageBox>
#include "glm.h"

extern QString filename;
extern bool fileChnaged;

class Midedge 
{
public:
	int tail,vertexnum,no;
	Midedge *midedge;

	Midedge() {
		midedge=NULL;
		tail=-1;
		vertexnum=-1;
		no=-1;
	}
};

class Vertex
{
public:
	Midedge *midedge;
	float axis[3];
	int num;

	Vertex() {
		midedge=NULL;
		num=0;
	}
};

class Triangle
{
public:
	Midedge *midedge;
	float norm[3];

	Triangle() {
		midedge=NULL;
	}

	void newedge(int num) {
		midedge=new Midedge[num];
	}
	void getnum(vector<Vertex> &ver) {
		float u[3],v[3];
		for(int i=0;i<3;i++) {
			u[i]=ver[midedge[1].tail].axis[i]-ver[midedge[0].tail].axis[i];
			v[i]=ver[midedge[2].tail].axis[i]-ver[midedge[0].tail].axis[i];
		}
		for(int i=0;i<3;i++) {
			norm[i]=u[(i+1)%3]*v[(i+2)%3]-u[(i+2)%3]*v[(i+1)%3];
		}
		float tmp=sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
		for(int i=0;i<3;i++) norm[i]=norm[i]/tmp;
	}
};

#endif