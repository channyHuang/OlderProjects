#ifndef DATA_H
#define DATA_H
#include "windows.h"
#include "gl/GL.h"
#include <gl/GLU.h>
#include <gl/glut.h>

#include <iostream>
#include <string>

#include <QString>

extern GLfloat lightPosition[3][4];
extern GLfloat lightShiness[3][1];
extern GLfloat lightAmbient[3][4];
extern GLfloat lightDiffuse[3][4];
extern GLfloat lightSpecular[3][4];
extern GLfloat lightEmission[3][4];

extern bool lightEnabled[3];

extern std::string backgroundname;

extern double ts, th;

extern QString timgname, tplyname, tmaskname;

#endif