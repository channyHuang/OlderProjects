#include "showtable4.h"
#include "ui_showtable4.h"
#include "data.h"

#include <QVBoxLayout>

showtable4::showtable4(QWidget *parent) 
	: ui(new Ui::showtable4),QWidget(parent)
{
	ui->setupUi(this);
	this->resize(totalwidth,totalheight);

	tableview=new QTableView;
	model=new QSqlTableModel(this);
	model->setTable("companyTable");
//	model->setTable("preCompanyTable");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	tableview->setModel(model);
	label=new QLabel(QStringLiteral("�ն����а�"));

	complabel = new QLabel(QStringLiteral("�³��ֵ��ն��У�"));
	compShow = new QLineEdit();
	compShow->setReadOnly(true);

	QHBoxLayout *inflayout = new QHBoxLayout;
	inflayout->addWidget(complabel);
	inflayout->addWidget(compShow);
	QVBoxLayout *tableLayout=new QVBoxLayout;
	tableLayout->addWidget(label);
	tableLayout->addWidget(tableview);
	QVBoxLayout *mainlayout=new QVBoxLayout;
	mainlayout->addLayout(inflayout);
	mainlayout->addLayout(tableLayout);
	setLayout(mainlayout);	
}
showtable4::~showtable4()
{
	delete ui;
}