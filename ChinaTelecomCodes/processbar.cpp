#include "processbar.h"
#include "ui_processbar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

processbar::processbar(QWidget *parent)
	: ui(new Ui::processbar),QWidget(parent)
{
	ui->setupUi(this);
	setWindowTitle(QStringLiteral("正在读取数据，请稍等……"));
	label=new QLabel;
	label->setText(QStringLiteral("数据比较大，需要一定的时间，请您耐心等候"));
	probar=new QProgressBar;
	probar->setRange(0,100);
	probar->setValue(99);

	QVBoxLayout *mainlayout=new QVBoxLayout;
	mainlayout->addWidget(label);
	mainlayout->addWidget(probar);
	setLayout(mainlayout);
}

processbar::~processbar()
{
	delete ui;
}