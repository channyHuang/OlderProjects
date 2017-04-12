#include "drawidget.h"
#include "data.h"

#include <QHBoxLayout>

drawidget::drawidget(QWidget *parent,int i)
{
	num=i;
	setWindowTitle("Multiple dim image:");

	initwidgets();
	initconnects();

	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addWidget(citylist);
	mainlayout->addLayout(stacklayout);
	setLayout(mainlayout);
}
drawidget::~drawidget()
{

}

void drawidget::initwidgets()
{
	citywindow = new cityDistWindow(NULL, num);

	stacklayout = new QStackedLayout;
	stacklayout->addWidget(citywindow);
	for (int i = 11; i < 42; i++)
		stacklayout->addWidget(new cityDistWindow(NULL, i));
	stacklayout->setCurrentIndex(0);

	citylist = new QListWidget();
	citylist->setFixedWidth(100);
	citylist->addItem("");
	for (int i = 0; i < province133.size(); i++) {
		citylist->addItem(province133[i]);
	}
}

void drawidget::initconnects()
{
	connect(citylist, SIGNAL(currentRowChanged(int)), stacklayout, SLOT(setCurrentIndex(int)));
}