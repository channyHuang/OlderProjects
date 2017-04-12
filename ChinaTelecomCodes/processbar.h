#ifndef PROCESSBAR_H
#define PROCESSBAR_H

#include <QtWidgets/QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>

namespace Ui
{
	class processbar;
}
class processbar : public QWidget
{
	Q_OBJECT
public:
	processbar(QWidget *parent=0);
	~processbar();

private:
	Ui::processbar *ui;

	QLabel *label;
	QProgressBar *probar;
	QTimer *timer;
};

#endif