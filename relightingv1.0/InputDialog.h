#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class InputDialog : public QDialog {
	Q_OBJECT
public:
	InputDialog(QWidget *parent = 0);

private:
	QLabel *imgLabel, *maskLabel;
	QPushButton *imgNameBtn, *imgMaskBtn, *modelBtn;
	QLineEdit *imgNameLine, *imgMaskLine, *modelLine;
	QPushButton *finishbtn;
	void initwidgets();
	void initconnects();

public slots: 
bool finishInput();

private slots:
bool selectImgName();
bool selectImgMask();
bool selectModel();
};

#endif