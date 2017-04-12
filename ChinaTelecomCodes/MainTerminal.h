#ifndef MAINTERMINAL_H
#define MAINTERMINAL_H

#include "ui_mainterminal.h"
#include <QWidget>
#include <QScrollArea>

namespace Ui
{
	class MainTerminal;
}

class MainTerminal : public QWidget
{
	Q_OBJECT
public:
	MainTerminal(QWidget *parent = 0);
	~MainTerminal();
private slots:
private:
	Ui::MainTerminal *ui;
	QScrollArea *area;
};

#endif