#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "ui_visualization.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStackedLayout>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>

namespace Ui {
	class visualization;
};
class visualization : public QWidget
{
	Q_OBJECT

public:
	visualization(QWidget *parent = 0);
	~visualization();

	void initializeWidget();
	void initializeConnect();
	void initdata();
private slots:
	void viewcitypath();
	void viewlogpath();
	void readcityinfo();
	void readloginfo();
	void showcityinfo();
	void showloginfo();

	void explorelog(); //output log table as excel

	void singleipimg();
	void singletimeimg();
	void singletypeimg();
	void singleurlimg();
	void singleresultimg();
	void singletelimg();
	void singlemobileimg();
	void singleuaimg();
	void singlecityimg();
	void showmulimg();

	void calculate();

	void getrule();
//
	void showActiveCustom();
	void showNewTerm();
	void showuvpvGraph();
private:
	Ui::visualization *ui;

	QLineEdit *citypathedit,*logpathedit;
	QPushButton *readcityinfobtn,*readloginfobtn;
	QPushButton *showcityinfobtn,*showloginfobtn;
	QPushButton *viewcitypathbtn,*viewlogpathbtn;

	QListWidget *chooselist;
	QStackedLayout *stacklayout;
	QPushButton *timebtn,*ipbtn,*telnumberbtn,*citybtn,*urlbtn,*UAbtn,*resultbtn,*typebtn,*mobilebtn;
	QCheckBox *checkbox1,*checkbox2,*checkbox3,*checkbox4,*checkbox5,*checkbox6,*checkbox7,*checkbox8,*checkbox9;
	QPushButton *showimgbtn;
	QWidget *singlepage,*multipage;

	QPushButton *exitbtn,*initbtn;
	QLabel *imglabel;

	QLabel *minsuplabel,*minconflabel,*rulelabel;
	QComboBox *rulealgorithm;
	QPushButton *getrules;
//add some function
	QPushButton *activeCustombtn, *newTermbtn;
//explore the log table as excel
	QPushButton *outputbtn;
//UV and PV
	QPushButton *uvpvbtn;

public:
	QComboBox *minsup,*minconf;

	QLabel *datatimetext, *datatimelabel1, *datatimelabel2;
};

#endif // VISUALIZATION_H
