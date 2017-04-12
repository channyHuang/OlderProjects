#include "showtable.h"
#include "ui_showtable.h"
#include "data.h"

#include <QVBoxLayout>

showtable::showtable(QWidget *parent) 
	: ui(new Ui::showtable),QWidget(parent)
{
	ui->setupUi(this);
	this->resize(totalwidth,totalheight);
	tableview=new QTableView;
	model=new QSqlTableModel(this);
	model->setTable("totalTable");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	tableview->setModel(model);
	label=new QLabel(QStringLiteral("日志数据表："));

	QVBoxLayout *loglayout=new QVBoxLayout;
	loglayout->addWidget(label);
	loglayout->addWidget(tableview);

	setLayout(loglayout);
}
showtable::~showtable()
{
	delete ui;
}