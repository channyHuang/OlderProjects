#include "cityDistWindow.h"

#include <QComboBox>
#include <QToolBar>
#include <QToolButton>

cityDistWindow::cityDistWindow(QWidget *parent, int num) 
	:QMainWindow(parent)
{
	d_plot = new Plot(parent, num);
	setCentralWidget(d_plot);

	QToolBar *toolBar = new QToolBar(this);

	QComboBox *typeBox = new QComboBox(toolBar);
	typeBox->addItem("Bars");
	typeBox->addItem("CandleSticks");
	typeBox->setCurrentIndex(1);
	typeBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QToolButton *btnExport = new QToolButton(toolBar);
	btnExport->setText("Export");
	btnExport->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	connect(btnExport, SIGNAL(clicked()), d_plot, SLOT(exportPlot()));

	toolBar->addWidget(typeBox);
	toolBar->addWidget(btnExport);
	addToolBar(toolBar);

	d_plot->setMode(typeBox->currentIndex());
	connect(typeBox, SIGNAL(currentIndexChanged(int)),
		d_plot, SLOT(setMode(int)));
}