#include "mainwin.h" 

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

#include <iostream>
using namespace std;

#include "data.h"

MainWin::MainWin(QWidget *parent)
	:ui(new Ui::MainWin), QWidget(parent)
{
	ui->setupUi(this);

	maskname = "./testdata/buddha/buddha.mask.png";
	plyname = "./testata/buddha.ply";

	setMinimumSize(960, 640);
	setMaximumSize(960, 640);

	initwidgets();
	initconnects();

	QPixmap pixmap = QPixmap(backgroundname.c_str());
	imglabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
	masklabel->setPixmap(QPixmap(maskname).scaled(100, 100, Qt::KeepAspectRatio));
	plylabel->setText(plyname);

	QVBoxLayout *widgetlayout = new QVBoxLayout();
	widgetlayout->addWidget(lightbtn);
	widgetlayout->addWidget(environmentbtn);
	gb->setLayout(widgetlayout);

	QVBoxLayout *btnlayout = new QVBoxLayout();
	btnlayout->addWidget(objlist);

	btnlayout->addWidget(imglabel);
	btnlayout->addWidget(chooseimgbtn);

	btnlayout->addWidget(masklabel);
	btnlayout->addWidget(choosemaskbtn);

	btnlayout->addWidget(plylabel);
	btnlayout->addWidget(chooseplybtn);

	btnlayout->addWidget(tsbox);
	btnlayout->addWidget(thbox);

	btnlayout->addWidget(gb);

	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addLayout(btnlayout);
	mainlayout->addWidget(tabwidget);

	setLayout(mainlayout);
}

MainWin::~MainWin()
{
	delete ui;
}

void MainWin::initwidgets()
{
	imglabel = new QLabel();
	imglabel->setMaximumSize(100, 100);
	masklabel = new QLabel();
	masklabel->setMaximumSize(100, 100);
	plylabel = new QLabel();
	plylabel->setMaximumSize(100, 100);

	chooseimgbtn = new QPushButton("Choose Image");
	choosemaskbtn = new QPushButton("Choose Mask");
	chooseplybtn = new QPushButton("Choose Ply");
	lightbtn = new QRadioButton("Light");
	environmentbtn = new QRadioButton("Environment");
	lightbtn->setChecked(true);

	objlist = new QListWidget();
	objlist->addItem("camera");
	objlist->addItem("Cow");
	objlist->addItem("dolphins");
	objlist->addItem("merilin");
	objlist->addItem("teapot");

	objlist->setFixedWidth(100);
	objlist->setFixedHeight(100);
	objlist->setCurrentRow(0);

	lightwidget = new lightWidget();
	environmentwidget = new environmentWidget();
	tabwidget = new QTabWidget;
	tabwidget->addTab(lightwidget, QString("Light"));
	tabwidget->addTab(environmentwidget, QString("Environment"));

	gb = new QGroupBox("widget:");

	tsbox = new QDoubleSpinBox();
	tsbox->setSingleStep(0.5);
	tsbox->setRange(0, 50);
	tsbox->setValue(1.0);
	thbox = new QDoubleSpinBox();
	thbox->setSingleStep(0.1);
	thbox->setRange(0, 1);
	thbox->setValue(0.3);
}

void MainWin::initconnects()
{
	connect(chooseimgbtn, SIGNAL(clicked()), this, SLOT(chooseimg()));
	connect(choosemaskbtn, SIGNAL(clicked()), this, SLOT(choosemask()));
	connect(chooseplybtn, SIGNAL(clicked()), this, SLOT(chooseply()));

	connect(lightbtn, SIGNAL(clicked()), this, SLOT(changetab()));
	connect(environmentbtn, SIGNAL(clicked()), this, SLOT(changetab()));

	connect(objlist, SIGNAL(currentTextChanged(QString)), this, SLOT(setObj(QString)));

	connect(tsbox, SIGNAL(valueChanged(double)), this, SLOT(paramvalue()));
	connect(thbox, SIGNAL(valueChanged(double)), this, SLOT(paramvalue()));
}

bool MainWin::chooseimg()
{
	QString imgname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;JPG(*.jpg);;BMP(*.bmp);;PNG(*.png)"));
	if (imgname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到图像文件..."));
		return false;
	}

	QPixmap pixmap = QPixmap(imgname);
	imglabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));

	backgroundname = imgname.toStdString();

	return true;
}

bool MainWin::choosemask()
{
	maskname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;JPG(*.jpg);;BMP(*.bmp);;PNG(*.png)"));
	if (maskname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到mask文件..., 将使用输入图像的所有像素。"));

		QPixmap pixmap = QPixmap(QSize(100, 100));
		pixmap.fill(Qt::white);
		masklabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
		return true;
	}

	QPixmap pixmap = QPixmap(maskname);
	masklabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));

	return true;
}

bool MainWin::chooseply()
{
	plyname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;PLY(*.ply);;obj(*.obj)"));
	if (plyname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到.ply或.obj点云文件..."));
		return false;
	}

	plylabel->setText(plyname);
	return true;
}

void MainWin::paramvalue()
{
	ts = tsbox->value();
	th = thbox->value();

	environmentwidget->init(backgroundname.c_str(), maskname.toStdString(), plyname.toStdString());
}

void MainWin::changetab()
{
	if (lightbtn->isChecked()) {
		tabwidget->setCurrentIndex(0);
	}
	else {
		tabwidget->setCurrentIndex(1);

		environmentwidget->init(backgroundname.c_str(), maskname.toStdString(), plyname.toStdString());
		//if (environmentbtn->isChecked()) environmentwidget->init(backgroundname.c_str(), maskname.toStdString(), plyname.toStdString());
	}
}

void MainWin::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_M:
		if (tabwidget->currentIndex() == 0) {
			tabwidget->setCurrentIndex(1);
		}
		else {
			tabwidget->setCurrentIndex(0);
		}
		cout << "Key: m" << endl;
		break;
	case Qt::Key_X:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->xrot -= 8.0f;
			if (lightwidget->mainWidget->xrot < -360) {
				lightwidget->mainWidget->xrot = 0;
			}
		}
		else {
			environmentwidget->glwidget->xrot -= 8.0f;
			if (environmentwidget->glwidget->xrot < -360) {
				environmentwidget->glwidget->xrot = 0;
			}
		}
		break;
	case Qt::Key_Y:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->yrot += 8.0f;
			if (lightwidget->mainWidget->yrot > 360) {
				lightwidget->mainWidget->yrot = 0;
			}
		}
		else {
			environmentwidget->glwidget->yrot += 8.0f;
			if (environmentwidget->glwidget->yrot > 360) {
				environmentwidget->glwidget->yrot = 0;
			}
		}
		break;
	case Qt::Key_Z:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->zrot += 8.0f;
			if (lightwidget->mainWidget->zrot > 360) {
				lightwidget->mainWidget->zrot = 0;
			}
		}
		else {
			environmentwidget->glwidget->zrot -= 8.0f;
			if (environmentwidget->glwidget->zrot > 360) {
				environmentwidget->glwidget->zrot = 0;
			}
		}
		break;
	case Qt::Key_S:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->scale -= 0.1f;
			if (lightwidget->mainWidget->scale < 0.1f) {
				lightwidget->mainWidget->scale = 1.0f;
			}
		}
		else {
			environmentwidget->glwidget->scale -= 0.1f;
			if (environmentwidget->glwidget->scale < 0.1f) {
				environmentwidget->glwidget->scale = 1.0f;
			}
		}
		break;
	case Qt::Key_U:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->ytra += 0.05f;
		}
		else {
			environmentwidget->glwidget->ytra += 0.05f;
		}
		break;
	case Qt::Key_D:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->ytra -= 0.05f;
		}
		else {
			environmentwidget->glwidget->ytra -= 0.05f;
		}
		break;
	case Qt::Key_L:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->xtra -= 0.05f;
		}
		else {
			environmentwidget->glwidget->xtra -= 0.05f;
		}
		break;
	case Qt::Key_R:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->xtra += 0.05f;
		}
		else {
			environmentwidget->glwidget->xtra += 0.05f;
		}
		break;
	case Qt::Key_Plus:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->ztra += 0.05f;
		}
		else {
			environmentwidget->glwidget->ztra += 0.05f;
		}
		break;
	case Qt::Key_Minus:
		if (tabwidget->currentIndex() == 0) {
			lightwidget->mainWidget->ztra -= 0.05f;
		}
		else {
			environmentwidget->glwidget->ztra -= 0.05f;
		}
		break;
	default:
		break;
	}
}

void MainWin::setObj(QString s)
{
	filename = QString("obj/") + s + QString(".obj");
	fileChnaged = true;
}