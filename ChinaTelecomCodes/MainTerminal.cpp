#include "MainTerminal.h"
#include "DrawTerminal.h"

#include <QVBoxLayout>

MainTerminal::MainTerminal(QWidget *parent /* = 0 */)
:ui(new Ui::MainTerminal), QWidget(parent)
{
	ui->setupUi(this);
	area = new QScrollArea;

	DrawTerminal *drawterminal = new DrawTerminal();
	area->setWidget(drawterminal);
	area->setGeometry(QRect(0, 0, 640, 480));

	QVBoxLayout *mainlayout = new QVBoxLayout();
	mainlayout->addWidget(area);
//	mainlayout->addWidget(drawterminal);

	setLayout(mainlayout);
}

MainTerminal::~MainTerminal()
{
	delete ui;
}