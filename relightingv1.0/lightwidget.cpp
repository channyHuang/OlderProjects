#include "lightwidget.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "data.h"

#include <iostream>
using namespace std;

lightWidget::lightWidget(QWidget *parent /* = 0 */)
{

	initWidgets();
	initConnects();

	QGridLayout *colorlayout = new QGridLayout();
	for (int i = 0; i < 3; i++) {
		colorlayout->addWidget(lightEnable[i], i * 9, 0, 1, 1);

		colorlayout->addWidget(diffusebtn[i], i * 9, 1, 1, 1);
		colorlayout->addWidget(ambientbtn[i], i * 9 + 1, 1, 1, 1);
		colorlayout->addWidget(new QLabel("Shiness "), i * 9 + 2, 0, 1, 1);
		colorlayout->addWidget(shinessslider[i], i * 9 + 2, 1, 1, 1);
		colorlayout->addWidget(specularbtn[i], i * 9 + 3, 1, 1, 1);
		colorlayout->addWidget(emissionbtn[i], i * 9 + 4, 1, 1, 1);

		colorlayout->addWidget(new QLabel("x"), i * 9 + 5 + 0, 0, 1, 1);
		colorlayout->addWidget(new QLabel("y"), i * 9 + 5 + 1, 0, 1, 1);
		colorlayout->addWidget(new QLabel("z"), i * 9 + 5 + 2, 0, 1, 1);
		for (int j = 0; j < 3; j++) {		

			colorlayout->addWidget(posslider[i][j], i * 9 + 5 + j, 1, 1, 1);
		}

		colorlayout->addWidget(new QLabel("- - - - -"), i * 9 + 8, 0, 1, 2);
	}
	
	/*
	QVBoxLayout *colorlayout = new QVBoxLayout();
	for (int i = 0; i < 3; i++) {
		colorlayout->addWidget(diffusebtn[i]);
		colorlayout->addWidget(ambientbtn[i]);
		colorlayout->addWidget(shinessslider[i]);
		colorlayout->addWidget(specularbtn[i]);	
		colorlayout->addWidget(emissionbtn[i]);
		for (int j = 0; j < 3; j++) {
			colorlayout->addWidget(posslider[i][j]);
		}		
	}
	*/
	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addWidget(mainWidget);
	mainlayout->addLayout(colorlayout);

	mainlayout->setStretchFactor(mainWidget, 4);
	mainlayout->setStretchFactor(colorlayout, 1);

	setLayout(mainlayout);
}

lightWidget::~lightWidget()
{

}

void lightWidget::initWidgets()
{
	for (int i = 0; i < 3; i++){

		lightEnable[i] = new QCheckBox();
		lightEnable[i]->setChecked(false);
		//color diffuse 
		diffusebtn[i] = new QPushButton(QString("Light ") + QString::number(i+1));
		diffusebtn[i]->setAutoFillBackground(true);
		diffusebtn[i]->setStyleSheet("color:black;background-color:rgb(255, 255, 255)");
		//shiness
		shinessslider[i] = new QSlider(Qt::Horizontal);
		shinessslider[i]->setRange(0, 100);
		shinessslider[i]->setValue(50);
		//color ambient
		ambientbtn[i] = new QPushButton(QString("Ambient ") + QString::number(i+1));
		ambientbtn[i]->setAutoFillBackground(true);
		ambientbtn[i]->setStyleSheet("color:black;background-color:rgb(255, 255, 255)");
		//color specular
		specularbtn[i] = new QPushButton("Specular " + QString::number(i+1));
		specularbtn[i]->setAutoFillBackground(true);
		specularbtn[i]->setStyleSheet("color:black;background-color:rgb(255, 255, 255)");
		//color emission
		emissionbtn[i] = new QPushButton("Emission " + QString::number(i+1));
		emissionbtn[i]->setAutoFillBackground(true);
		emissionbtn[i]->setStyleSheet("color:black;background-color:rgb(255, 255, 255)");

	//position

		for (int j = 0; j < 3; j++) {
			posslider[i][j] = new QDoubleSpinBox();
			posslider[i][j]->setValue(0.0);
			posslider[i][j]->setSingleStep(0.5);
			posslider[i][j]->setRange(-100, 100);
		}
	}
	
	lightEnable[0]->setChecked(true);

	mainWidget = new MainGLWidget();
}

void lightWidget::initConnects()
{
	for (int i = 0; i < 3; i++) {
		connect(diffusebtn[i], SIGNAL(clicked()), this, SLOT(setColor()));
		connect(shinessslider[i], SIGNAL(valueChanged(int)), this, SLOT(setShiness()));
		connect(specularbtn[i], SIGNAL(clicked()), this, SLOT(setColor()));
		connect(emissionbtn[i], SIGNAL(clicked()), this, SLOT(setColor()));
		connect(ambientbtn[i], SIGNAL(clicked()), this, SLOT(setColor()));

		for (int j = 0; j < 3; j++) {
			connect(posslider[i][j], SIGNAL(valueChanged(double)), this, SLOT(setPosition()));
		}
		
		connect(lightEnable[i], SIGNAL(clicked()), this, SLOT(setLight()));
	}
}

void lightWidget::setColor()
{
	QColor color = QColorDialog::getColor(color, this);
	int rgb[3] = { 0 };
	rgb[0] = color.red();
	rgb[1] = color.green();
	rgb[2] = color.blue();

	QString string = QString("color:black;background-color:rgb(%1,%2,%3)").arg(rgb[0]).arg(rgb[1]).arg(rgb[2]);

	QPushButton *btn = qobject_cast<QPushButton*>(sender());
	for (int i = 0; i < 3; i++) {
		if (btn == diffusebtn[i]) {
			diffusebtn[i]->setStyleSheet(string);
			for (int j = 0; j < 3; j++) {
				lightDiffuse[i][j] = rgb[j] / 255.0;
			}			
		}

		else if (btn == specularbtn[i]) {
			specularbtn[i]->setStyleSheet(string);
			for (int j = 0; j < 3; j++) {
				lightSpecular[i][j] = rgb[j] / 255.0;
			}
		}

		else if (btn == emissionbtn[i]) {
			emissionbtn[i]->setStyleSheet(string);
			for (int j = 0; j < 3; j++) {
				lightEmission[i][j] = rgb[j] / 255.0;
			}
		}

		else if (btn == ambientbtn[i]) {
			ambientbtn[i]->setStyleSheet(string);
			for (int j = 0; j < 3; j++) {
				lightAmbient[i][j] = rgb[j] / 255.0;
			}
		}
	}

}

void lightWidget::setShiness()
{
	for (int i = 0; i < 3; i++) {
		lightShiness[i][0] = shinessslider[i]->value();
	}
}

void lightWidget::setPosition()
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			lightPosition[i][j] = posslider[i][j]->value();
		}
	}
}

void lightWidget::setLight()
{
	for (int i = 0; i < 3; i++) {
		if (lightEnable[i]->isChecked()) {
			lightEnabled[i] = true;
		}
		else {
			lightEnabled[i] = false;
		}
	}
}