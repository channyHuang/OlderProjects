#ifndef TIMECITY_H
#define TIMECITY_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace Ui
{
	class timeCity;
}

class timeCity : public QWidget
{
	Q_OBJECT
public:
	timeCity(QWidget *parent = 0);
	~timeCity();
private slots:
	bool showImage();
private:
	Ui::timeCity *ui;

	QListWidget *datelist;
	QPushButton *btn;
	void calculate();
	
};

#endif