#include "InputDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
using namespace std;

#include "data.h"

InputDialog::InputDialog(QWidget *parent) {
	setWindowTitle("input");
	initwidgets();
	initconnects();

	QHBoxLayout *imgLayout = new QHBoxLayout();
	imgLayout->addWidget(imgNameBtn);
	imgLayout->addWidget(imgNameLine);
	QHBoxLayout *maskLayout = new QHBoxLayout();
	maskLayout->addWidget(imgMaskBtn);
	maskLayout->addWidget(imgMaskLine);
	QHBoxLayout *modelLayout = new QHBoxLayout();
	modelLayout->addWidget(modelBtn);
	modelLayout->addWidget(modelLine);
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(imgLayout);
	mainLayout->addWidget(imgLabel);
	mainLayout->addLayout(maskLayout);
	mainLayout->addWidget(maskLabel);
	mainLayout->addLayout(modelLayout);
	mainLayout->addWidget(finishbtn);

	setLayout(mainLayout);
}

void InputDialog::initwidgets() {
	imgNameBtn = new QPushButton("Image Name");
	imgMaskBtn = new QPushButton("Image Mask");
	modelBtn = new QPushButton("Model");
	imgNameLine = new QLineEdit();
	imgMaskLine = new QLineEdit();
	modelLine = new QLineEdit();

	finishbtn = new QPushButton("Finish");

	imgLabel = new QLabel();
	maskLabel = new QLabel();
}

void InputDialog::initconnects() {
	connect(imgNameBtn, SIGNAL(clicked()), this, SLOT(selectImgName()));
	connect(imgMaskBtn, SIGNAL(clicked()), this, SLOT(selectImgMask()));
	connect(modelBtn, SIGNAL(clicked()), this, SLOT(selectModel()));
	connect(finishbtn, SIGNAL(clicked()), this, SLOT(finishInput()));
}

bool InputDialog::selectImgName() {
	timgname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;JPG(*.jpg);;BMP(*.bmp);;PNG(*.png)"));
	if (timgname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到图像文件..."));
		return false;
	}

	imgNameLine->setText(timgname);
	QPixmap pixmap = QPixmap(timgname);
	imgLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));

	return true;
}

bool InputDialog::selectImgMask() {
	tmaskname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;JPG(*.jpg);;BMP(*.bmp);;PNG(*.png)"));
	if (tmaskname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到mask文件..., 将使用输入图像的所有像素。"));

		QPixmap pixmap = QPixmap(QSize(100, 100));
		pixmap.fill(Qt::white);
		maskLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
		return true;
	}

	imgMaskLine->setText(tmaskname);
	QPixmap pixmap = QPixmap(tmaskname);
	maskLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));

	return true;
}

bool InputDialog::selectModel() {
	tplyname = QFileDialog::getOpenFileName(this, tr("des Image"), "", tr("ALL files(*.*);;PLY(*.ply);;OBJ(*.obj)"));
	if (tplyname.isEmpty()) {
		QMessageBox::warning(NULL, "error!", QStringLiteral("找不到.ply或.obj点云文件..."));
		return false;
	}

	modelLine->setText(tplyname);
	return true;
}

bool InputDialog::finishInput() {
	if (tplyname == NULL || tmaskname == NULL || timgname == NULL) {
		QMessageBox::warning(this, "Warning", "Target name is empty, default value will be used.");
		return false;
	}
	this->close();
	return true;
}