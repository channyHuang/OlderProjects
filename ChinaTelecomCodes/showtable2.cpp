#include "showtable2.h"
#include "ui_showtable2.h"
#include "data.h"

#include <QVBoxLayout>

showtable2::showtable2(QWidget *parent) 
	: ui(new Ui::showtable2),QWidget(parent)
{
	ui->setupUi(this);
	this->resize(totalwidth,totalheight);

	table133=new QTableView;
	model133=new QSqlTableModel(this);
	model133->setTable("telToCity133");
	model133->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model133->select();
	table133->setModel(model133);
	label133=new QLabel(QStringLiteral("133开头手机号所在城市："));

	table153=new QTableView;
	model153=new QSqlTableModel(this);
	model153->setTable("telToCity153");
	model153->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model153->select();
	table153->setModel(model153);
	label153=new QLabel(QStringLiteral("153开头手机号所在城市"));

	table189=new QTableView;
	model189=new QSqlTableModel(this);
	model189->setTable("telToCity189");
	model189->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model189->select();
	table189->setModel(model189);
	label189=new QLabel(QStringLiteral("189开头手机号所在城市"));

	QVBoxLayout *table133layout=new QVBoxLayout;
	table133layout->addWidget(label133);
	table133layout->addWidget(table133);
	QVBoxLayout *table153layout=new QVBoxLayout;
	table153layout->addWidget(label153);
	table153layout->addWidget(table153);
	QVBoxLayout *table189layout=new QVBoxLayout;
	table189layout->addWidget(label189);
	table189layout->addWidget(table189);
	QHBoxLayout *mainlayout=new QHBoxLayout;
	mainlayout->addLayout(table133layout);
	mainlayout->addLayout(table153layout);
	mainlayout->addLayout(table189layout);
	setLayout(mainlayout);	
}
showtable2::~showtable2()
{
	delete ui;
}