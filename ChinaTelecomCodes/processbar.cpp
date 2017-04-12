#include "processbar.h"
#include "ui_processbar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

processbar::processbar(QWidget *parent)
	: ui(new Ui::processbar),QWidget(parent)
{
	ui->setupUi(this);
	setWindowTitle(QStringLiteral("���ڶ�ȡ���ݣ����Եȡ���"));
	label=new QLabel;
	label->setText(QStringLiteral("���ݱȽϴ���Ҫһ����ʱ�䣬�������ĵȺ�"));
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