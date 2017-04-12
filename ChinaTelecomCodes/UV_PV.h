#ifndef UV_PV_H
#define UV_PV_H

#include <QListWidget>
#include <QPushButton>

namespace Ui
{
	class UVPVwidget;
}

class UVPVwidget : public QWidget
{
	Q_OBJECT
public:
	UVPVwidget(QWidget *parent = 0);
	~UVPVwidget();

private slots:
	bool showImage();
	bool singleShow();
private:
	Ui::UVPVwidget *ui;

	QListWidget *termlist;
	QPushButton *showbtn, *singleshowbtn;

	void calculate();
};

#endif