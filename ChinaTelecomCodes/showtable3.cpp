#include "showtable3.h"
#include "ui_showtable3.h"
#include "data.h"

#include <QVBoxLayout>

showtable3::showtable3(QWidget *parent) 
	: ui(new Ui::showtable3),QWidget(parent)
{
	ui->setupUi(this);
	this->resize(totalwidth,totalheight);

	tableview=new QTableView;
	model=new QSqlTableModel(this);
	model->setTable("activeCustom");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	tableview->setModel(model);
	label=new QLabel(QStringLiteral("活跃用户排行榜："));

	QVBoxLayout *tableLayout=new QVBoxLayout;
	tableLayout->addWidget(label);
	tableLayout->addWidget(tableview);
	QHBoxLayout *mainlayout=new QHBoxLayout;
	mainlayout->addLayout(tableLayout);
	setLayout(mainlayout);	
}
showtable3::~showtable3()
{
	delete ui;
}