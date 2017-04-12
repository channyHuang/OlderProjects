#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "mainGLWidget.h"

class lightWidget : public QWidget
{
	Q_OBJECT
public:
	lightWidget(QWidget *parent = 0);
	~lightWidget();

	MainGLWidget *mainWidget;
protected:
	void initWidgets();
	void initConnects();
protected slots:
	void setColor();
	void setShiness();
	void setPosition();
	void setLight();
private:
	QPushButton *diffusebtn[3], *specularbtn[3], *ambientbtn[3], *emissionbtn[3];

	QSlider *shinessslider[3];
	QDoubleSpinBox *posslider[3][3];

	QCheckBox *lightEnable[3];
};

#endif