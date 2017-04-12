#ifndef DRAWIDGET_H
#define DRAWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedLayout>

#include "cityDistWindow.h"

class drawidget : public QWidget
{
	Q_OBJECT
public:
	drawidget(QWidget *parent=0,int i=1);
	~drawidget();

	void initwidgets();
	void initconnects();

protected:
	QListWidget *citylist;
	QStackedLayout *stacklayout;

	int num;

	cityDistWindow *citywindow;
};

#endif