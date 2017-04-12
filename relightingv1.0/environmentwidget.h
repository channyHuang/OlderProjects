#ifndef ENVIRONMENTWIDGET_H
#define ENVIRONMENTWIDGET_H

#include <QWidget>
#include "GLWidget.h"

#include "RelightingLine/estimate.h"

class environmentWidget : public QWidget
{
	Q_OBJECT
public:
	environmentWidget(QWidget *parent = 0);
	~environmentWidget();

	GLWidget *glwidget;

	void init(QString imgname, string maskname, string plyname);
private:
	Estimation *est;

	void test();
};

#endif