#include "data.h"

GLfloat lightPosition[3][4] = { { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 } };
GLfloat lightShiness[3][1] = { { 50 }, { 50 }, { 50 } };
GLfloat lightAmbient[3][4] = { { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 } };
GLfloat lightDiffuse[3][4] = { { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 } };
GLfloat lightSpecular[3][4] = { { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 } };
GLfloat lightEmission[3][4] = { { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0, 1.0 } };

bool lightEnabled[3] = { true, false, false };

std::string backgroundname = "./testdataset/buddha/buddha.0.png";

double ts = 50;
double th = 0.7;

QString timgname = "./testdataset/cat/cat.3.png", tplyname = "./testdataset/cat.ply", tmaskname = "./testdataset/cat/cat.mask.png";