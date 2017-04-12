#include "timeCity.h"
#include "ui_timeCity.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include "database.h"
#include "data.h"
//#include "Python.h"


#include <fstream>
using namespace std;

timeCity::timeCity(QWidget *parent)
:ui(new Ui::timeCity),QWidget(parent)
{
	if (begintime.year == 0) {
		timespan();
	}

	datelist = new QListWidget();
	connect(datelist, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
	btn = new QPushButton("Show Image");
	connect(btn, SIGNAL(clicked()), this, SLOT(showImage()));

	for (int i = 0; i < datatime.size(); i++) {
		datelist->addItem(QStringLiteral("%1-%2-%3").arg(datatime[i].year).arg(datatime[i].month).arg(datatime[i].date));
	}

	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addWidget(datelist);
	mainlayout->addWidget(btn);
	
	setLayout(mainlayout);
}

timeCity::~timeCity()
{
	delete ui;
}

void timeCity::calculate()
{
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

	int no = datelist->currentRow();
	fstream fs("timeCity.txt", fstream::out | ios_base::trunc);
	QSqlQuery query;
	int sum133[31] = { 0 }, sum189[31] = { 0 };
	query.exec(QString("drop view cityview"));
	query.exec(QString("create view cityview as select hourss,telnumber,city from totalTable where years=%1 and months=%2 and dates=%3 and city<>-1").arg(datatime[no].year).arg(datatime[no].month).arg(datatime[no].date));
	for (int j = 0; j < 24; j++) {
		query.exec(QString("drop view subcityview"));
		query.exec(QString("create view subcityview as select * from cityview where hourss=%1").arg(j));
		for (int k = 0; k < 31; k++) {
			query.exec(QString("select count(*) from subcityview where (telnumber like '133%' or telnumber like '153%') and city>=%1 and city<%2").arg(numofcity133[j]).arg(numofcity133[j + 1]));
			query.next();
			sum133[j] = query.value(0).toInt();
			query.exec(QString("select count(*) from subcityview where telnumber like '189%' and city>=%1 and city<%2").arg(numofcity189[j]).arg(numofcity189[j + 1]));
			query.next();
			sum189[j] += query.value(0).toInt();
		}
		for (int i = 0; i < 31; i++) {
			switch (i) {
			case 0:sum133[18] += sum189[i]; break;
			case 1:sum133[9] += sum189[i]; break;
			case 2:sum133[10] += sum189[i]; break;
			case 3:sum133[21] += sum189[i]; break;
			case 4:sum133[12] += sum189[i]; break;
			case 5:sum133[16] += sum189[i]; break;
			case 6:sum133[8] += sum189[i]; break;
			case 7:sum133[17] += sum189[i]; break;
			case 8:sum133[11] += sum189[i]; break;
			case 9:sum133[25] += sum189[i]; break;
			case 10:sum133[13] += sum189[i]; break;
			case 11:sum133[19] += sum189[i]; break;
			case 12:sum133[22] += sum189[i]; break;
			case 13:sum133[26] += sum189[i]; break;
			case 15:sum133[23] += sum189[i]; break;
			case 16:sum133[24] += sum189[i]; break;
			case 17:sum133[29] += sum189[i]; break;
			case 18:sum133[5] += sum189[i]; break;
			case 19:sum133[0] += sum189[i]; break;
			case 20:sum133[2] += sum189[i]; break;
			case 21:sum133[15] += sum189[i]; break;
			case 22:sum133[20] += sum189[i]; break;
			case 23:sum133[1] += sum189[i]; break;
			case 24:sum133[7] += sum189[i]; break;
			case 25:sum133[6] += sum189[i]; break;
			case 26:sum133[3] += sum189[i]; break;
			case 29:sum133[4] += sum189[i]; break;
			default:sum133[i] += sum189[i]; break;
			}
			fs << " " << sum133[i];
		}	
		fs << endl;
	}

	fs.close();
}

bool timeCity::showImage()
{
	calculate();
/*	Py_SetPythonHome("D:/python27");
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
	pFunc = PyDict_GetItemString(pDict, "timeCity");
	if (!pFunc || !PyCallable_Check(pFunc))
	{
		printf("can't find function [timeCity]");
		getchar();
		return false;
	}
	PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pName);
	Py_DECREF(pArgs);
	Py_DECREF(pModule);

	Py_Finalize();*/

	QString runPath = QCoreApplication::applicationDirPath();
	QString cmd = QString("python.exe %1/ChinaTest.py 1").arg(runPath);
	system(cmd.toLocal8Bit());

//	system("python.exe ChinaTest.py 1");
//	system("pause");

	return true;
}