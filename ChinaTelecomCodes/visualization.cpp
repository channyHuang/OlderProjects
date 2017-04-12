#include "visualization.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QRegExp>
#include <QTextDocumentWriter>

#include "mainwindow.h"
#include "database.h"
#include "showtable.h"
#include "showtable2.h"
#include "showtable3.h"
#include "showtable4.h"
#include "processbar.h"
#include <cassert>
#include "data.h"
#include "apriori.h"
#include "FPtree.h"
#include "PRtree.h"

#include "time.h"
#include "windows.h"
#include "MainTerminal.h"
#include "DrawTerminal.h"
#include "UV_PV.h"
visualization::visualization(QWidget *parent)
	: ui(new Ui::visualization), QWidget(parent)
{
	ui->setupUi(this);
	this->setWindowTitle(QStringLiteral("中国电信日志数据可视化"));
	this->setMinimumSize(totalwidth,totalheight);
	this->setMaximumSize(QSize(totalwidth,totalheight));
	QFile file("./style.qss");
	file.open(QFile::ReadOnly);
	setStyleSheet(QString(file.readAll()));
	file.close();

	initdata();
	initializeWidget();
	initializeConnect();

	//整体布局设计模块
	QVBoxLayout *singlelayout=new QVBoxLayout;
	singlelayout->addWidget(ipbtn);
	singlelayout->addWidget(timebtn);
	singlelayout->addWidget(typebtn);
	singlelayout->addWidget(urlbtn);
	singlelayout->addWidget(resultbtn);
	singlelayout->addWidget(telnumberbtn);
	singlelayout->addWidget(mobilebtn);
	singlelayout->addWidget(UAbtn);
	singlelayout->addWidget(citybtn);
	
	QVBoxLayout *multilayout=new QVBoxLayout;
	multilayout->addWidget(checkbox1);
	multilayout->addWidget(checkbox2);
	multilayout->addWidget(checkbox3);
	multilayout->addWidget(checkbox4);
	multilayout->addWidget(checkbox5);
	multilayout->addWidget(checkbox6);
	multilayout->addWidget(checkbox7);
	multilayout->addWidget(checkbox8);
	multilayout->addWidget(checkbox9);
	multilayout->addWidget(showimgbtn);

	singlepage = new QWidget;
	singlepage->setLayout(singlelayout);
	multipage = new QWidget;
	multipage->setLayout(multilayout);
	stacklayout = new QStackedLayout;
	stacklayout->addWidget(singlepage);
	stacklayout->addWidget(multipage);
	stacklayout->setCurrentIndex(0);

	QHBoxLayout *readcitylayout=new QHBoxLayout;
	readcitylayout->addWidget(citypathedit);
	readcitylayout->addWidget(viewcitypathbtn);
	readcitylayout->addWidget(readcityinfobtn);
	readcitylayout->addWidget(showcityinfobtn);
	QHBoxLayout *readloglayout=new QHBoxLayout;
	readloglayout->addWidget(logpathedit);
	readloglayout->addWidget(viewlogpathbtn);
	readloglayout->addWidget(readloginfobtn);
	readloglayout->addWidget(showloginfobtn);
	QVBoxLayout *btnlayout=new QVBoxLayout;
	btnlayout->addWidget(outputbtn);

	btnlayout->addWidget(initbtn);
	btnlayout->addWidget(chooselist);
	btnlayout->addStretch();
	btnlayout->addLayout(stacklayout);
	btnlayout->addWidget(exitbtn);
	QVBoxLayout *rulelayout=new QVBoxLayout;
	rulelayout->addWidget(datatimetext);
	rulelayout->addWidget(datatimelabel1);
	rulelayout->addWidget(datatimelabel2);

	rulelayout->addWidget(activeCustombtn);
	rulelayout->addWidget(newTermbtn);
	rulelayout->addWidget(uvpvbtn);

	rulelayout->addWidget(minsuplabel);
	rulelayout->addWidget(minsup);
	rulelayout->addWidget(minconflabel);
	rulelayout->addWidget(minconf);
	rulelayout->addWidget(rulelabel);
	rulelayout->addWidget(rulealgorithm);
	rulelayout->addWidget(getrules);
	QHBoxLayout *hlayout=new QHBoxLayout;
	hlayout->addLayout(rulelayout);
	hlayout->addWidget(imglabel);
	hlayout->addLayout(btnlayout);
	QVBoxLayout *mainLayout=new QVBoxLayout;
	mainLayout->addLayout(readcitylayout);
	mainLayout->addLayout(readloglayout);
	mainLayout->addLayout(hlayout);
	setLayout(mainLayout);

	connect(chooselist, SIGNAL(currentRowChanged(int)), stacklayout, SLOT(setCurrentIndex(int)));
}

visualization::~visualization()
{
	delete ui;
}
//************************************** Initialize ***********************************
void visualization::initdata()
{
	begintime.year = 0;
}

void visualization::initializeWidget()
{
	//主体图像显示
	imglabel = new QLabel();
	imglabel->setPixmap(QPixmap::fromImage(QImage("./image/background.jpg")));
	imglabel->resize(480, 320);
	//数据读取与显示模块
	citypathedit = new QLineEdit();
	citypathedit->setPlaceholderText(QStringLiteral("请输入城市文件路径："));
	logpathedit = new QLineEdit();
	logpathedit->setPlaceholderText(QStringLiteral("请输入日志文件路径："));
	viewcitypathbtn = new QPushButton();
	viewcitypathbtn->setIcon(QIcon("./image/view.png"));
	viewlogpathbtn = new QPushButton();
	viewlogpathbtn->setIcon(QIcon("./image/view.png"));
	readcityinfobtn = new QPushButton(QStringLiteral("读取号码与城市信息"));
	readloginfobtn = new QPushButton(QStringLiteral("读取日志文件"));
	showcityinfobtn = new QPushButton(QStringLiteral("显示号码与城市信息"));
	showloginfobtn = new QPushButton(QStringLiteral("显示原始日志数据"));
	initbtn = new QPushButton(QStringLiteral("统计数据"));
	exitbtn = new QPushButton(QStringLiteral("退出程序"));

	outputbtn = new QPushButton(QStringLiteral("导出日志文件为Excel"));

	//关联规则挖掘
	rulealgorithm = new QComboBox();
	rulealgorithm->addItem("");
	rulealgorithm->addItem("Apriori");
	rulealgorithm->addItem("FP-tree");
	rulealgorithm->addItem("PR-tree");
	minsup = new QComboBox();
	minsup->addItem("60");
	minconf = new QComboBox();
	minconf->addItem("80");
	minsup->setEditable(true);
	minconf->setEditable(true);
	QRegExp rx("[1-9][0-9]");
	QValidator *validator = new QRegExpValidator(rx, 0);
	minsup->lineEdit()->setValidator(validator);
	minconf->lineEdit()->setValidator(validator);

	minsuplabel = new QLabel(QStringLiteral("支持度："));
	minconflabel = new QLabel(QStringLiteral("置信度："));
	rulelabel = new QLabel(QStringLiteral("选择关联规则挖掘算法"));
	getrules = new QPushButton(QStringLiteral("获取关联规则"));

	//交互模块
	chooselist = new QListWidget;
	chooselist->addItem(QStringLiteral("单维度变量分析"));
	chooselist->addItem(QStringLiteral("多维度变量分析"));
	ipbtn = new QPushButton(QStringLiteral("ip"));
	timebtn = new QPushButton(QStringLiteral("时间"));
	typebtn = new QPushButton(QStringLiteral("页面提交方式"));
	urlbtn = new QPushButton(QStringLiteral("url"));
	resultbtn = new QPushButton(QStringLiteral("访问返回值"));
	telnumberbtn = new QPushButton(QStringLiteral("电话号码"));
	mobilebtn = new QPushButton(QStringLiteral("操作系统"));
	UAbtn = new QPushButton(QStringLiteral("厂商"));
	citybtn = new QPushButton(QStringLiteral("城市分布"));

	checkbox1 = new QCheckBox;
	checkbox1->setText(QStringLiteral("ip"));
	checkbox2 = new QCheckBox;
	checkbox2->setText(QStringLiteral("时间"));
	checkbox3 = new QCheckBox;
	checkbox3->setText(QStringLiteral("访问方式"));
	checkbox4 = new QCheckBox;
	checkbox4->setText(QStringLiteral("url"));
	checkbox5 = new QCheckBox;
	checkbox5->setText(QStringLiteral("访问返回值"));
	checkbox6 = new QCheckBox;
	checkbox6->setText(QStringLiteral("电话号码"));
	checkbox7 = new QCheckBox;
	checkbox7->setText(QStringLiteral("机型"));
	checkbox8 = new QCheckBox;
	checkbox8->setText(QStringLiteral("浏览器"));
	checkbox9 = new QCheckBox;
	checkbox9->setText(QStringLiteral("城市"));
	showimgbtn = new QPushButton(QStringLiteral("显示图表"));

	//活跃用户
	activeCustombtn = new QPushButton(QStringLiteral("活跃用户"));
	newTermbtn = new QPushButton(QStringLiteral("新出现的终端"));

	datatimetext = new QLabel(QStringLiteral("数据集时间段："));
	datatimelabel1 = new QLabel(QStringLiteral("起始时间"));
	datatimelabel2 = new QLabel(QStringLiteral("终止时间"));
	//UV and PV 
	uvpvbtn = new QPushButton(QStringLiteral("UV-PV图"));
}
void visualization::initializeConnect()
{
	connect(initbtn, SIGNAL(clicked()), this, SLOT(calculate()));
	connect(exitbtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(viewlogpathbtn, SIGNAL(clicked()), this, SLOT(viewlogpath()));
	connect(viewcitypathbtn, SIGNAL(clicked()), this, SLOT(viewcitypath()));
	connect(readloginfobtn, SIGNAL(clicked()), this, SLOT(readloginfo()));
	connect(readcityinfobtn, SIGNAL(clicked()), this, SLOT(readcityinfo()));
	connect(showcityinfobtn, SIGNAL(clicked()), this, SLOT(showcityinfo()));
	connect(showloginfobtn, SIGNAL(clicked()), this, SLOT(showloginfo()));

	connect(outputbtn, SIGNAL(clicked()), this, SLOT(explorelog()));

	connect(getrules, SIGNAL(clicked()), this, SLOT(getrule()));


	connect(ipbtn, SIGNAL(clicked()), this, SLOT(singleipimg()));
	connect(timebtn, SIGNAL(clicked()), this, SLOT(singletimeimg()));
	connect(typebtn, SIGNAL(clicked()), this, SLOT(singletypeimg()));
	connect(urlbtn, SIGNAL(clicked()), this, SLOT(singleurlimg()));
	connect(resultbtn, SIGNAL(clicked()), this, SLOT(singleresultimg()));
	connect(telnumberbtn, SIGNAL(clicked()), this, SLOT(singletelimg()));
	connect(mobilebtn, SIGNAL(clicked()), this, SLOT(singlemobileimg()));
	connect(UAbtn, SIGNAL(clicked()), this, SLOT(singleuaimg()));
	connect(citybtn, SIGNAL(clicked()), this, SLOT(singlecityimg()));

	connect(showimgbtn, SIGNAL(clicked()), this, SLOT(showmulimg()));
	

	connect(activeCustombtn, SIGNAL(clicked()), this, SLOT(showActiveCustom()));
	connect(newTermbtn, SIGNAL(clicked()), this, SLOT(showNewTerm()));

	connect(uvpvbtn, SIGNAL(clicked()), this, SLOT(showuvpvGraph()));
}
//************************************** Read data and display***********************************
void visualization::viewcitypath()
{
	QString filepath=QFileDialog::getExistingDirectory(this,tr("Choose the path of log files:"),QDir::currentPath(),QFileDialog::ShowDirsOnly);
	if(!filepath.isEmpty()) citypathedit->setText(filepath);
}

void visualization::viewlogpath()
{
	QString filepath=QFileDialog::getExistingDirectory(this,tr("Choose the path of city files:"),QDir::currentPath(),QFileDialog::ShowDirsOnly);
	if(!filepath.isEmpty()) logpathedit->setText(filepath);
}

void visualization::readloginfo()
{
	QString path=logpathedit->text();
	if(path=="") {QMessageBox::warning(NULL,"error","Please select the path of log files!");return;}
	Database db;
	assert(db.open());
	QSqlQuery query;
	query.exec(QString("delete from totalTable"));
	query.exec(QString("delete from sqlite_sequence where name='totalTable'"));
	processbar *pro=new processbar;
	pro->show();
	db.readloginfo(path);
	pro->deleteLater();
	db.close();
}

void visualization::readcityinfo()
{
	QString path=citypathedit->text();
	if(path=="") {QMessageBox::warning(NULL,"error","Please select the path of city files!");return;}
	Database db;
	assert(db.open());
	QSqlQuery query;
	query.exec(QString("delete from telToCity133"));
	query.exec(QString("delete from sqlite_sequence where name='telToCity133'"));
	query.exec(QString("delete from telToCity133"));
	query.exec(QString("delete from sqlite_sequence where name='telToCity153'"));
	query.exec(QString("delete from telToCity133"));
	query.exec(QString("delete from sqlite_sequence where name='telToCity189'"));
	processbar *pro=new processbar;
	pro->show();
	db.readcityinfo(path);
	pro->deleteLater();
	db.close();
}

void visualization::showloginfo()
{
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	showtable *table=new showtable;
	table->show();
}

void visualization::showcityinfo()
{
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db") ) {QMessageBox::warning(this,"error!","Please read the city files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("telToCity133")) QMessageBox::warning(NULL,"miss:","table telToCity133 does not exit!");
	if(!tables.contains("telToCity153")) QMessageBox::warning(NULL,"miss:","table telToCity153 does not exit!");
	if(!tables.contains("telToCity189")) QMessageBox::warning(NULL,"miss:","table telToCity189 does not exit!");
	showtable2 *table=new showtable2;
	table->show();
}
//explore the log file to .csv file, this can be used to change to .xls(Excel) file
void visualization::explorelog()
{
	const QString file_path = "./test.csv";
	QFile csvFile(file_path);
	if (!csvFile.open(QIODevice::ReadWrite)) {
		return;
	}

	QFile *databasefile = new QFile;
	if (!databasefile->exists("./database.db")) { QMessageBox::warning(this, "error!", "Please read the log files first!"); return; }
	Database db;
	db.open();

	QSqlQuery query;
	query.exec("select * from totalTable");

	while (query.next()) {
		csvFile.write(query.value(0).toString().toLocal8Bit());
		for (int i = 1; i < 16; i++) {
			csvFile.write("\t" + query.value(i).toString().toLocal8Bit());
		}
		csvFile.write("\n");
	}
	csvFile.close();
//************************************ The file too large, out of memory **************************************
/*	QFile *databasefile = new QFile;
	if (!databasefile->exists("./database.db")) { QMessageBox::warning(this, "error!", "Please read the log files first!"); return; }
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("database.db");
	db.setUserName("capg");
	db.setPassword("123");
	db.open();
	QSqlQuery query;
	query.exec("select * from totalTable");
	QString begin = QString::fromLocal8Bit("<HTML><HEAD></HEAD><BODY><TABLE CELLSPACING=0 cellpadding=10 COLS=17  BORDER=1 ><TR><TD COLSPAN=17  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=8 COLOR=RED>标题</TD></TR><TR><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4 > ip </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 年 </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 月 </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 日 </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 时 </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 分 </TD><TD  ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 秒 </TD> \
										   										   		<TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 访问类型 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> url </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 返回结果 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 待定 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 字节数 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 电话号码 </TD> \
																																				<TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 操作系统 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 厂商 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 城市 </TD><TD ALIGN=CENTER VALIGN=MIDDLE><FONT SIZE=4> 源 </TD></TR>");
	QString mid = QString::fromLocal8Bit("<TR><TD  ALIGN=CENTER VALIGN=MIDDLE >%1</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%2</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%3</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%4</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%5</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%6</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%7</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%8</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%9</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%10</TD> \
										 										 										 <TD  ALIGN=CENTER VALIGN=MIDDLE >%11</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%12</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%13</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%14</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%15</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%16</TD><TD  ALIGN=CENTER VALIGN=MIDDLE >%17</TD></TR>");
	QString end = QString::fromLocal8Bit("</TABLE></BODY></HTML>");
	while (query.next()) {
		QString midtmp = mid.arg(query.value(0).toString()).arg(query.value(1).toInt()).arg(query.value(2).toInt()).arg(query.value(3).toInt()).arg(query.value(4).toInt()).arg(query.value(5).toInt()).arg(query.value(6).toInt()).arg(query.value(7).toString()).arg(query.value(8).toString()).arg(query.value(9).toString()).arg(query.value(10).toString()) \
			.arg(query.value(11).toString()).arg(query.value(12).toString()).arg(query.value(13).toString()).arg(query.value(14).toString()).arg(query.value(15).toString()).arg(query.value(16).toString());
		begin.append(midtmp);
	}
	begin.append(end);
	db.close();
//	textEdit->setText(begin);

	QString fn = QFileDialog::getSaveFileName(this, tr("Save as…"),
		QString(), tr("EXCEL files (*.xls);;ODS files (*.ods);;ODF files (*.odt);;HTML-Files (*.htm *.html);;All Files (*)"));
	if (fn.isEmpty())
		return;
	if (!(fn.endsWith(".xls", Qt::CaseInsensitive) || fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive) || fn.endsWith(".ods", Qt::CaseInsensitive)))
		fn += ".xls"; // default
	if (fn.endsWith(".odt", Qt::CaseInsensitive)){
		QTextDocumentWriter writer(fn);
//		writer.write(textEdit->document());
	}
	else{
		QFile index(fn);
		if (index.open(QFile::WriteOnly | QIODevice::Text))
		{
			QTextStream liu(&index);
			liu.setCodec("UTF-8");
//			liu << textEdit->document()->toHtml("UTF-8");
		}
	}
	*/
}

//********************************************** Single dimantion ***************************************
void visualization::singleipimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	//统计并显示数据
	memset(&ipcount,0,sizeof(ipcount));
	int a,b;
	QSqlQuery query;
	query.exec("select count(*) from totalTable group by ip");
	QString tmp;
	while(query.next()) {
		tmp=query.value(1).toString();
		sscanf((const char*)tmp.toLocal8Bit(),"%d.%d",&a,&b);
		ipcount[a*256+b].count = query.value(0).toInt();
		ipcount[a*256+b].firstnum=a;
		ipcount[a*256+b].secondnum=b;
	}
	windows *w=new windows(this,1);
	w->show();
}

void visualization::singletimeimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	//统计并显示数据
	memset(hourcount,0,sizeof(hourcount));
	memset(monthcount,0,sizeof(monthcount));
	memset(yearcount,0,sizeof(yearcount));
	memset(datecount,0,sizeof(datecount));
	earlyyear=0;
	QSqlQuery query;
	query.exec("select count(*) from totalTable group by years");
	query.next();
	earlyyear=query.value(1).toInt();
	yearcount[0] = query.value(0).toInt();
	while(query.next()) {
		yearcount[query.value(1).toInt()-earlyyear] = query.value(0).toInt();;
	}
	query.exec("select count(*) from totalTable group by months");
	while(query.next()) {
		monthcount[query.value(1).toInt()-1] = query.value(0).toInt();
	}
	query.exec("select count(*) from totalTable group by dates");
	while(query.next()) {
		datecount[query.value(1).toInt()-1] = query.value(0).toInt();
	}
	query.exec("select count(*) from totalTable group by hourss");
	while(query.next()) {
		hourcount[query.value(1).toInt()/6] = query.value(0).toInt();
	}
	windows *w=new windows(this,2);
	w->show();
}

void visualization::singletypeimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	//统计并显示数据
	QSqlQuery query;
	query.exec("select count(*) from totalTable group by sitetype");
	query.next();
	getnum=query.value(0).toInt();
	query.next();
	postnum=query.value(0).toInt();
	windows *w=new windows(this,3);
	w->show();
	db.close();
}

void visualization::singleurlimg()
{
	windows *w=new windows(this,4);
	w->show();
}

void visualization::singleresultimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	//统计并显示数据
	QSqlQuery query;
	query.exec("select count(*) from totalTable where result=200 or result=302 or result=304 group by result");
	query.next();
	re200=query.value(0).toInt();
	query.next();
	re302=query.value(0).toInt();
	query.next();
	re304=query.value(0).toInt();
	windows *w=new windows(this,5);
	w->show();
}

void visualization::singletelimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("totalTable")) {QMessageBox::warning(NULL,"wrong data:","no table totalTable");return;}
	//统计并显示数据
	QSqlQuery query;
	query.exec("select count(*) from totalTable where telnumber like '133%'");
	query.next();
	telcount[0]=query.value(0).toInt();
	query.exec("select count(*) from totalTable where telnumber like '153%'");
	query.next();
	telcount[1] = query.value(0).toInt();
	query.exec("select count(*) from totalTable where telnumber like '189%'");
	query.next();
	telcount[2] = query.value(0).toInt();
	windows *w=new windows(this,6);
	w->show();
}

void visualization::singlemobileimg()
{
	windows *w=new windows(this,7);
	w->show();
}

void visualization::singleuaimg()
{
	windows *w=new windows(this,8);
	w->show();
}

void visualization::singlecityimg()
{
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(this,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("telToCity133")||!tables.contains("telToCity153")||!tables.contains("telToCity189")) {QMessageBox::warning(NULL,"wrong data:","no city information!\nPlease read the city files first!");return;}
//	db.setcity();
	//统计并显示数据
	windows *w=new windows(this,9);
	w->show();
}

void visualization::showmulimg()
{
	int checkint = 100;
	if (checkbox2->isChecked() && checkbox9->isChecked())
		checkint = 10;
	else if (checkbox2->isChecked())
		checkint = 11;
	windows *w=new windows(this,checkint);
	w->show();
}

void visualization::calculate()
{

	totalrecord=0;
	no=0;
	//判断数据是否存在
	QFile *databasefile=new QFile;
	if(!databasefile->exists("./database.db")) {QMessageBox::warning(NULL,"error!","Please read the log files first!");return;}
	Database db;
	db.open();
	QStringList tables;
	tables=db.db.tables();
	if(!tables.contains("telToCity133")||!tables.contains("telToCity153")||!tables.contains("telToCity189")) {
		QMessageBox::warning(NULL,"wrong data:","no city information!\nPlease read the city files first!");
		return;
	}
	//统计数据
	QString tmp;
	
	memset(ua,0,sizeof(ua));
//	earlyyear=0;
	QSqlQuery query;
	query.exec("select * from totalTable");
	while(query.next()) {
/*		//**************************************ip**************************************
		tmp=query.value(0).toString();
		sscanf((const char*)tmp.toLocal8Bit(),"%d.%d",&a,&b);
		ipcount[a*256+b].count++;
		ipcount[a*256+b].firstnum=a;
		ipcount[a*256+b].secondnum=b;	
		//**************************************time**************************************
		if(earlyyear==0) earlyyear=query.value(1).toInt();
		yearcount[query.value(1).toInt()-earlyyear]++;
		monthcount[query.value(2).toInt()-1]++;
		datecount[query.value(3).toInt()-1]++;
		hourcount[query.value(4).toInt()/6]++;
		*/
		//**************************************url**************************************
		tmp=query.value(14).toString();
		int i;
		for(i=0;i<no;i++) {
			if(strcmp((const char *)urlcount[i].url.toLocal8Bit(),(const char *)tmp.toLocal8Bit())==0) {
				urlcount[i].count++;
				break;
			}
		}
		if(i==no) {
			urlcount[no].url=tmp;
			urlcount[no].count=1;
			no++;
		}
		//**************************************tel and city**************************************
		tmp=query.value(12).toString();
		if(tmp[1]=='3') {
		//	telcount[0]++;
			citycount133[citynum((const char*)tmp.toLocal8Bit())]++;
		}
		else if(tmp[1]=='5') {
		//	telcount[1]++;
			citycount133[citynum((const char*)tmp.toLocal8Bit())]++;
		}
		else if(tmp[1]=='8') {
		//	telcount[2]++;
			citycount189[citynum((const char*)tmp.toLocal8Bit())]++;
		}
		//**************************************mobile**************************************
		tmp=query.value(13).toString();
		if(tmp.contains("Android")) os[0]++;
		else if(tmp.contains("Linux")) os[1]++;
		else if(tmp.contains("Windows")) os[2]++;
		else if(tmp.contains("Bada")) os[3]++;
		//**************************************ua**************************************
		for(int i=0;i<10;i++) 
			if(tmp.contains(uachar[i])) {ua[i]++;break;}
		//****************************************************************************
		totalrecord++;
	}

	int j=0,k=0;
	for(int i=0;i<31;i++) {
		for(;k<numofcity189[i];k++) totalcitycount189[i]+=citycount189[k];
		switch(i) {
		case 0:totalcitycount133[18] += totalcitycount189[i]; break;
		case 1:totalcitycount133[9] += totalcitycount189[i]; break;
		case 2:totalcitycount133[10] += totalcitycount189[i]; break;
		case 3:totalcitycount133[21] += totalcitycount189[i]; break;
		case 4:totalcitycount133[12] += totalcitycount189[i]; break;
		case 5:totalcitycount133[16] += totalcitycount189[i]; break;
		case 6:totalcitycount133[8] += totalcitycount189[i]; break;
		case 7:totalcitycount133[17] += totalcitycount189[i]; break;
		case 8:totalcitycount133[11] += totalcitycount189[i]; break;
		case 9:totalcitycount133[25] += totalcitycount189[i]; break;
		case 10:totalcitycount133[13] += totalcitycount189[i]; break;
		case 11:totalcitycount133[19] += totalcitycount189[i]; break;
		case 12:totalcitycount133[22] += totalcitycount189[i]; break;
		case 13:totalcitycount133[26] += totalcitycount189[i]; break;
		case 15:totalcitycount133[23] += totalcitycount189[i]; break;
		case 16:totalcitycount133[24] += totalcitycount189[i]; break;
		case 17:totalcitycount133[29] += totalcitycount189[i]; break;
		case 18:totalcitycount133[5] += totalcitycount189[i]; break;
		case 19:totalcitycount133[0] += totalcitycount189[i]; break;
		case 20:totalcitycount133[2] += totalcitycount189[i]; break;
		case 21:totalcitycount133[15] += totalcitycount189[i]; break;
		case 22:totalcitycount133[20] += totalcitycount189[i]; break;
		case 23:totalcitycount133[1] += totalcitycount189[i]; break;
		case 24:totalcitycount133[7] += totalcitycount189[i]; break;
		case 25:totalcitycount133[6] += totalcitycount189[i]; break;
		case 26:totalcitycount133[3] += totalcitycount189[i]; break;
		case 29:totalcitycount133[4] += totalcitycount189[i]; break;
		default:totalcitycount133[i] += totalcitycount189[i]; break;
		}
		for(;j<numofcity133[i];j++)	totalcitycount133[i]+=citycount133[j];
	}

	if (begintime.year == 0) {
		timespan();
	}
	//显示时间段
	datatimelabel1->setText(QString("%1-%2-%3").arg(begintime.year).arg(begintime.month).arg(begintime.date));
	datatimelabel2->setText(QString("%1-%2-%3").arg(endtime.year).arg(endtime.month).arg(endtime.date));

}
//the time span of datasets

//********************************************** rules ***************************************
void visualization::getrule() {
	minSup=minsup->currentText().toInt();
	minConf=minconf->currentText().toInt();
//	long start_time,end_time;
	LARGE_INTEGER start_time,end_time;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start_time);
	switch(rulealgorithm->currentIndex()) {		
	case 1: {
		apriori *alg1=new apriori();
		alg1->action();
		break;
			}
	case 2: {
		fptree *alg2=new fptree();
		alg2->action();
		break;
			}
	case 3: {
		prtree *alg3=new prtree();
		alg3->action();
		break;
			}
	default: break;
	}
	QueryPerformanceCounter(&end_time);
	ofstream of;
	of.open("./testdata/time.txt");
	of<<"time:"<<(double)(end_time.QuadPart-start_time.QuadPart)/(double)frequency.QuadPart<<endl;
	of.close();
}
//Active customers detected
void visualization::showActiveCustom()
{
	Database db;
	db.open();
	QStringList tables;
	tables = db.db.tables();
	if (!tables.contains("totalTable")) { QMessageBox::warning(NULL, "wrong data:", "no city information!\nPlease read the city files first!"); return; }
	//统计用户访问次数
	QSqlQuery query;
	query.exec(QString("drop table activeCustom"));
	query.exec(QString("create table activeCustom as select telnumber, count(*) from totalTable group by telnumber order by count(*) desc"));

	tables = db.db.tables();
	if (!tables.contains("activeCustom")) { QMessageBox::warning(NULL, "wrong data:", "no table activeCustom"); return; }
	showtable3 *table = new showtable3;
	table->show();
}
//Capture new terminal
void visualization::showNewTerm()
{
	Database db;
	db.open();
	QStringList tables;
	tables = db.db.tables();
	if (!tables.contains("totalTable")) { QMessageBox::warning(NULL, "wrong data:", "no city information!\nPlease read the city files first!"); return; }
	db.close();
	//select time
	if (datatime.size() == 0) {
		timespan();
	}
	db.open();
	QSqlQuery query;
	dataTime tmpDatatime;
	datacompany.clear();
	list<dataCompany> tmpDatacompany;
	dataCompany tmpcompany;
	for (int i = 0; i < datatime.size(); i++) {
		tmpDatatime = datatime.at(i);
		query.exec(QString("drop view view_%1").arg(i));
		query.exec(QString("create view view_%4 as select company,count(*) from totalTable where years=%1 and months=%2 and dates=%3 group by company").arg(tmpDatatime.year).arg(tmpDatatime.month).arg(tmpDatatime.date).arg(i));

		query.exec(QString("select company,count(*) from totalTable where years=%1 and months=%2 and dates=%3 group by company").arg(tmpDatatime.year).arg(tmpDatatime.month).arg(tmpDatatime.date));


		QFile csvFile("./Terminal-" + QString::number(tmpDatatime.year) + "-" + QString::number(tmpDatatime.month) + "-" + QString::number(tmpDatatime.date) + ".csv");
		if (!csvFile.open(QIODevice::ReadWrite)) {
			return;
		}

		tmpDatacompany.clear();
		while (query.next()) {
			csvFile.write(query.value(0).toString().toLocal8Bit() + "\t" + query.value(1).toString().toLocal8Bit() + "\n");
			tmpcompany.name = query.value(0).toString().toLocal8Bit();
			tmpcompany.num = query.value(1).toInt();
			tmpDatacompany.push_back(tmpcompany);
		}
		datacompany.push_back(tmpDatacompany);

		csvFile.close();
	}

	list<dataCompany> newlistcomp;
	dataCompany newcomp;
	for (int i = 3; i < datatime.size(); i++) {
		newlistcomp.clear();
//		query.exec(QString("select * from view_1 where company not in (select company from view_0)"));
		QString tmp = QString("select * from view_%1 where company not in (select company from view_%2)").arg(i).arg(i - 1);
		for (int j = i - 2; j >= 0; j--) {
			tmp += QString("and company not in (select company from view_%1)").arg(j);
		}
		query.exec(tmp);
		
		while (query.next())	{
			newcomp.name = query.value(0).toString().toLocal8Bit();
			newcomp.num = query.value(1).toInt();
			newlistcomp.push_back(newcomp);
		}
		newtml.push_back(newlistcomp);
	}

	db.close();

	MainTerminal *mainterminal = new MainTerminal();
	mainterminal->show();
/*	Database db;
	db.open();
	QStringList tables;
	tables = db.db.tables();
	if (!tables.contains("totalTable")) { QMessageBox::warning(NULL, "wrong data:", "no city information!\nPlease read the city files first!"); return; }
	//统计终端出现次数
	QSqlQuery query;
	query.exec(QString("drop table preCompanyTable"));
	if (tables.contains("companyTable")) 
		query.exec(QString("create table preCompanyTable as select company from companyTable order by company"));
	query.exec(QString("drop table companyTable"));
	query.exec(QString("create table companyTable as select company, count(*) from totalTable group by company order by count(*) desc"));
	
	tables = db.db.tables();
	if (!tables.contains("companyTable")) { QMessageBox::warning(NULL, "wrong data:", "no table companyTable"); return; }
	//对比查看是否有新终端出现
	QString tmps = "";
	int count = 0;
	query.exec(QString("select company from companyTable where company not in (select company from preCompanyTable)"));
	while (query.next()) {
		tmps += query.value(0).toString();
		count++;
	}
	showtable4 *table = new showtable4;
	if (count == 0) table->compShow->setText(QStringLiteral("没有新出现的终端..."));
	else table->compShow->setText(tmps);
	table->show();
	*/
}
//Draw UV-PV graph for special terminal
void visualization::showuvpvGraph()
{
/*	if (begintime.year == 0) {
		timespan();
	}
*/
	UVPVwidget *myUVPVwidget = new UVPVwidget();
	myUVPVwidget->show();
}