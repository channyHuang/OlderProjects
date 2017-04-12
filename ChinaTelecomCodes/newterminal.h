#ifndef NEWTERMINAL_H
#define NEWTERMINAL_H

#include <QWidget>

namespace Ui
{
	class Terminal;
};

class Terminal : public QWidget
{
	Q_OBJECT
public:
	Terminal::Terminal(QWidget *parent = 0);
	~Terminal();

private:
	Ui::Terminal *ui;
};


#endif