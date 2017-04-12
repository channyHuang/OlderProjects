#include "mainGLWidget.h"

#include "data.h"

#include <iostream>

MainGLWidget::MainGLWidget(QWidget *parent /* = 0 */) : QGLWidget(parent)
{
	xrot = 0.0;
	yrot = 0.0;
	zrot = 0.0;

	xtra = 0.0;
	ytra = 0.0;
	ztra = 0.0;

	scale = 0.5;
	
	setAutoBufferSwap(false);
}

MainGLWidget::~MainGLWidget()
{

}

void MainGLWidget::initializeGL()
{
	model.load(filename);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	gluPerspective(45.0f, 1, 0.1f, 100.0f);
	
	glEnable(GL_LIGHTING);
	
	glEnable(GL_DEPTH_TEST);
}

void MainGLWidget::initLight()
{
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse[0]);
	glLightfv(GL_LIGHT0, GL_SHININESS, lightShiness[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular[0]);
	glLightfv(GL_LIGHT0, GL_EMISSION, lightEmission[0]);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse[1]);
	glLightfv(GL_LIGHT1, GL_SHININESS, lightShiness[1]);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient[1]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular[1]);
	glLightfv(GL_LIGHT1, GL_EMISSION, lightEmission[1]);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse[2]);
	glLightfv(GL_LIGHT2, GL_SHININESS, lightShiness[2]);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient[2]);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular[2]);
	glLightfv(GL_LIGHT2, GL_EMISSION, lightEmission[2]);
}

void MainGLWidget::resizeGL(int w, int h)
{
	if (h == 0) h = 1;
	glViewport(0, 0, (GLint)w, (GLint)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MainGLWidget::paintGL()
{
	if (fileChnaged) model.load(filename);
	fileChnaged = false;

	initLight();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	initBackground();
	drawBackground();
	
	glPushMatrix();
	glRotatef(xrot, 1.0, 0.0, 0.0);
	glRotatef(yrot, 0.0, 1.0, 0.0);
	glRotatef(zrot, 0.0, 0.0, 1.0);

	glTranslatef(xtra, ytra, ztra);

	glPushMatrix();
	glScaled(scale, scale, scale);
	displaymodel();
	glPopMatrix();

	//*************** 灯光0
	glPushMatrix();
	glRotatef(lightzrot, 0.0, 0.0, -1.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition[0]);
	if (lightEnabled[0]) glEnable(GL_LIGHT0);   //使用指定灯光
	else glDisable(GL_LIGHT0);
	glPopMatrix();
	
	//*************** 灯光1
	glPushMatrix();
	glRotatef(lightxrot, 1.0, 0.0, 0.0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition[1]);
	if (lightEnabled[1]) glEnable(GL_LIGHT1);   //使用指定灯光
	else glDisable(GL_LIGHT1);
	glPopMatrix();

	//*************** 灯光2
	glPushMatrix();
	glRotatef(lightyrot, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition[2]);
	if (lightEnabled[2]) glEnable(GL_LIGHT2);   //使用指定灯光
	else glDisable(GL_LIGHT2);
	glPopMatrix();
	
	glPopMatrix();

	swapBuffers();
	update();
}

void MainGLWidget::displaymodel()
{
	if (model.tri.size() <= 0) return;
	Triangle *mytri;
	for (int i = 0; i < model.tri.size(); i++) {
		mytri = &model.tri[i];
		glBegin(GL_TRIANGLES);
		glNormal3fv(mytri->norm);
		glVertex3fv(model.ver[mytri->midedge[0].tail].axis);
		glVertex3fv(model.ver[mytri->midedge[1].tail].axis);
		glVertex3fv(model.ver[mytri->midedge[2].tail].axis);
		glEnd();
	}
}

void MainGLWidget::initBackground()
{
	m_Background.load(backgroundname.c_str());
	m_Background.scaled(QSize(100, 100));


}

void MainGLWidget::drawBackground()
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, 640, 480, m_Background);
}