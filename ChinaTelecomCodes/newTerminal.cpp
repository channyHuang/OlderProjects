#include "newTerminal.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

Terminal::Terminal(QWidget *parent /* = 0 */)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);
}

Terminal::~Terminal()
{
	delete ui;
}