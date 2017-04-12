#ifndef SINGLEWIDGET_H
#define SINGLEWIDGET_H

#include <QtWidgets/QWidget>

class SingleWidget : public QWidget
{
	Q_OBJECT
public:
	SingleWidget(QWidget *parent=0,int i=0);
protected:
	void paintEvent(QPaintEvent *event);
private:
	int num;
};

#endif