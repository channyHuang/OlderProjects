#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QAction>

#include "drawidget.h"
#include "singlewidget.h"
#include "timeCity.h"

namespace Ui
{
	class windows;
}
class windows : public QMainWindow
{
	Q_OBJECT
public:
	windows(QWidget *parent=0,int i=1);
	~windows();

	private slots:
		bool saveimg();
//		bool saveasimg();
		void printimg();
		void copyimg();
		void about();
private:
	QAction *saveaction,*saveasaction,*closeaction,*aboutaction,*printaction,*copyaction;
	Ui::windows *ui;
	QString curfile;

	drawidget *glwidget;
	SingleWidget *singlwidget;
	timeCity *timecity;

	bool savefile(const QString &filename);
	int num;
};

#endif