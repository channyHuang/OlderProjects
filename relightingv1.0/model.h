#ifndef MODEL_H
#define MODEL_H

#include "objdata.h"

#include <QString>

class Model
{
public:
	Model();
	bool load(QString filename);

	void copyvector(float d[3],float s[3]) {	for(int i=0;i<3;i++) d[i]=s[i];	}
	void copyvectori(int d[3],GLuint s[3]) {	for(int i=0;i<3;i++) d[i]=s[i];	}

	void calculate(float re[3],float point1[3],float point2[3],float point3[3],float point4[3]) {
		for(int i=0;i<3;i++) re[i]=(point1[i]+point2[i])*0.375+(point3[i]+point4[i])*0.125;
	}
	void calculate(float re[3],float point1[3],float point2[3]) {
		for(int i=0;i<3;i++) re[i]=(point1[i]+point2[i])*0.5;
	}

	vector<Triangle> tri;
	vector<Vertex> ver;
	int edgenum;
	float tx,ty,tz;
};

#endif