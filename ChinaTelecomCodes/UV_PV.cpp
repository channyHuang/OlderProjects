#include "UV_PV.h"
#include "ui_UVPVwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include "data.h"
#include "database.h"
//#include "Python.h"

#include <fstream>
using namespace std;

UVPVwidget::UVPVwidget(QWidget *parent /* = 0 */)
:ui(new Ui::UVPVwidget), QWidget(parent)
{

	termlist = new QListWidget();
	connect(termlist, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
	/*
		QFile *databasefile = new QFile;
		if (!databasefile->exists("./database.db")) { QMessageBox::warning(NULL, "error!", "Please read the log files first!"); return; }
		Database db;
		db.open();
		QStringList tables;
		tables = db.db.tables();
		if (!tables.contains("totalTable")) {
		QMessageBox::warning(this, "Warning", "Time span too large...");
		return;
		}
		QSqlQuery query;
		query.exec("select distinct company from totalTable");
		ofstream term("terminal.txt", fstream::out | ios_base::trunc);
		while (query.next()) {
		term << query.value(0).toString().toStdString() << endl;
		termlist->addItem(QString("%1").arg(query.value(0).toString()));
		}
		termlist->setCurrentRow(0);
		term.close();
		db.close();
		*/
	string str;
	ifstream fs("terminal.txt");
	while (getline(fs, str)) {
		termlist->addItem(str.c_str());
	}
	termlist->setCurrentRow(0);

	singleshowbtn = new QPushButton("special terminal");
	connect(singleshowbtn, SIGNAL(clicked()), this, SLOT(singleShow()));
	showbtn = new QPushButton("Show Image");
	connect(showbtn, SIGNAL(clicked()), this, SLOT(showImage()));

	QHBoxLayout *listlayout = new QHBoxLayout();
	listlayout->addWidget(termlist);
	listlayout->addWidget(singleshowbtn);
	listlayout->addWidget(showbtn);
	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addLayout(listlayout);

	setLayout(mainlayout);
}

UVPVwidget::~UVPVwidget()
{
	delete ui;
}

void UVPVwidget::calculate()
{
/*	if (datatime.size() == 0) {
		timespan();
	}
*/	QString str/* = termlist->currentItem()->text()*/;
	Database db;
	db.open();
	QSqlQuery query;

	ofstream mulf("multiTerm.txt", fstream::out | ios_base::trunc);
	int sum1 = 0, sum2 = 0;

	/*
	for (int j = 0; j < 5; j++) {
		str = termlist->item(j)->text();
		query.exec("drop view singleTerm");
		query.exec(QString("create view singleTerm as select * from totalTable where company='%1'").arg(str));
		for (int i = 1; i < 4; i++) {
			query.exec(QString("select count(*),count(distinct userip) from singleTerm where years=2013 and months=1 and dates=%1 group by hourss").arg(i));
			while (query.next())	{
				sum1 += query.value(0).toInt();
				sum2 += query.value(1).toInt();
				mulf << sum2 << " " << sum1 << endl;
			}
		}
		mulf << endl;
	}
	*/
	for (int j = 0; j < termlist->count(); j++) {
		str = termlist->item(j)->text();
		query.exec("drop view singleTerm");
		query.exec(QString("create view singleTerm as select * from totalTable where company='%1'").arg(str));
		for (int i = 0; i < datatime.size(); i++) {
			query.exec(QString("select count(*),count(distinct userip) from singleTerm where years=%1 and months=%2 and dates=%3 group by hourss").arg(datatime[i].year).arg(datatime[i].month).arg(datatime[i].date));
			while (query.next())	{
				sum1 += query.value(0).toInt();
				sum2 += query.value(1).toInt();
				mulf << sum2 << " " << sum1 << endl;
			}
		}
		mulf << endl;
	}
	
	mulf.close();
	db.close();
}

bool UVPVwidget::singleShow()
{
	if (datatime.size() == 0) {
		timespan();
	}
	QString str = termlist->currentItem()->text();
	Database db;
	db.open();
	QSqlQuery query;
	int sum1 = 0, sum2 = 0;
	ofstream sigf("singleTerm.txt", fstream::out | ios_base::trunc);
	sigf << sum2 << " " << sum1 << endl;

	for (int i = 0; i < datatime.size(); i++) {
		sum1 = 0;
		sum2 = 0;
		query.exec("drop view singleTerm");
		query.exec(QString("create view singleTerm as select userip,hourss from totalTable where years=%1 and months=%2 and dates=%3 and company='%4'").arg(datatime[i].year).arg(datatime[i].month).arg(datatime[i].date).arg(str));
		query.exec("select count(*),count(distinct userip) from singleTerm group by hourss");
		while (query.next())	{
			sum1 += query.value(0).toInt();
			sum2 += query.value(1).toInt();
			sigf << sum2 << " " << sum1 << endl;
		}
		sigf << endl;
	}

	sigf.close();
	db.close();
	/*
	Py_SetPythonHome("D:/python27");
	Py_Initialize();
	if (!Py_IsInitialized())	{
		return false;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs;
	pName = PyString_FromString("ChinaTest");
	pModule = PyImport_Import(pName);
	if (!pModule)
	{
		printf("can't find ChinaTest.py");
		getchar();
		return false;
	}
	pDict = PyModule_GetDict(pModule);
	if (!pDict)
	{
		return false;
	}
	pFunc = PyDict_GetItemString(pDict, "singleUVPV");
	if (!pFunc || !PyCallable_Check(pFunc))
	{
		printf("can't find function [singleUVPV]");
		getchar();
		return false;
	}
	PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pName);
	Py_DECREF(pArgs);
	Py_DECREF(pModule);

	Py_Finalize();*/
	QString runPath = QCoreApplication::applicationDirPath();
	QString cmd = QString("python.exe %1/ChinaTest.py 2").arg(runPath);
	system(cmd.toLocal8Bit());
//	system("python.exe ChinaTest.py 2");

	return true;
}

bool UVPVwidget::showImage()
{
	calculate();
	/*
	Py_SetPythonHome("D:/python27");
	Py_Initialize();
	if (!Py_IsInitialized())	{
		return false;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs;
	pName = PyString_FromString("ChinaTest");
	pModule = PyImport_Import(pName);
	if (!pModule)
	{
		printf("can't find ChinaTest.py");
		getchar();
		return false;
	}
	pDict = PyModule_GetDict(pModule);
	if (!pDict)
	{
		return false;
	}
	pFunc = PyDict_GetItemString(pDict, "UVPV");
	if (!pFunc || !PyCallable_Check(pFunc))
	{
		printf("can't find function [UVPV]");
		getchar();
		return false;
	}
	PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pName);
	Py_DECREF(pArgs);
	Py_DECREF(pModule);

	Py_Finalize();*/
	QString runPath = QCoreApplication::applicationDirPath();
	QString cmd = QString("python.exe %1/ChinaTest.py 3").arg(runPath);
	system(cmd.toLocal8Bit());
//	system("python.exe ChinaTest.py 3");

	return true;
}