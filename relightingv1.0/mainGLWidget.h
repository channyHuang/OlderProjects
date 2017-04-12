#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include <QPainter>

#include "model.h"

class MainGLWidget : public QGLWidget
{
	Q_OBJECT
public:
	MainGLWidget(QWidget *parent = 0);
	~MainGLWidget();

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

public:
	

	Model model;

	GLfloat xrot, yrot, zrot;
	GLfloat lightxrot, lightyrot, lightzrot;

	GLfloat xtra, ytra, ztra;
	GLfloat scale;
};

#endif 