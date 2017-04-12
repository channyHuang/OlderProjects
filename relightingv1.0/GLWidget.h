#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QPixmap>

#include "model.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT
public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();

	GLfloat xrot, yrot, zrot;
	GLfloat xtra, ytra, ztra;
	GLfloat scale;
protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
private:
	void initLight();
	void displaymodel();

	void initBackground();
	void drawBackground();

	QPixmap m_Background;

	Model model;

	GLfloat lightxrot, lightyrot, lightzrot;
	
};

#endif