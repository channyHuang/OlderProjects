#ifndef MAINWIN_H
#define MAINWIN_H

#include <QtWidgets/QWidget>
#include "ui_mainwin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QListWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QGroupBox>
#include <QDoubleSpinBox>

#include <string>
using namespace std;

#include "lightwidget.h"
#include "environmentwidget.h"

namespace Ui
{
	class MainWin;
}

class MainWin : public QWidget
{
	Q_OBJECT

public:
	MainWin(QWidget *parent = 0);
	~MainWin();

	void initwidgets();
	void initconnects();

	QListWidget *objlist;

private slots:
	bool chooseimg();
	bool choosemask();
	bool chooseply();

	void paramvalue();

	void changetab();

	void setObj(QString s);
protected:
	void keyPressEvent(QKeyEvent *e);
private:
	Ui::MainWin *ui;

	QLabel *imglabel, *masklabel, *plylabel;
	QPushButton *chooseimgbtn, *choosemaskbtn, *chooseplybtn;
	QRadioButton *lightbtn, *environmentbtn;
	QGroupBox *gb;
//	QString imgname;

	QTabWidget *tabwidget;
	

	lightWidget *lightwidget;
	environmentWidget *environmentwidget;

	QString maskname, plyname;

	QDoubleSpinBox *tsbox, *thbox;
};

#endif // MAINWIN_H
