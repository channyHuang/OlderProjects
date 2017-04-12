#include "database.h"
#include <QMessageBox>
#include <QString>
#include <QDir>
#include "data.h"
#include <QMessageBox>
#include <zlib.h>
#include <iostream>
#include <string>
using namespace std;

Database::Database()
{

}
Database::~Database()
{
	close();
}
bool Database::open()
{
	db=QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("database.db");
	db.setUserName("capg");
	db.setPassword("123");
	return db.open();
}
void Database::close()
{
	db.close();
}
void Database::readloginfo(QString path)
{
	//路径
//	QString path="F:/serve";
//	totalrecord=0;
	QSqlQuery query;
	query.exec(QObject::tr("create table totalTable(userip char(16),years int,months int,dates int,hourss int,minites int,seconds int,sitetype char(5),url char(2048),result int,subpro char(60),bytes int,telnumber char(15),os char(10),company char(10),city int,sources char(2048))"));
	//读取服务器日志
	QDir dir(path);
	if(!dir.exists()) {
		QMessageBox::warning(NULL,"error","can not found the path!");
		return ;
	}
//	dir.setFilter(QDir::Dirs|QDir::Files);
	dir.setFilter(QDir::Files);
//	dir.setSorting(QDir::DirsFirst);
	QFileInfoList list=dir.entryInfoList();
	int i=0;
	char buf[4096];
	QString fulfile;
	do{
		totalrecord=0;
		QFileInfo fileInfo=list.at(i);
		fulfile=path+"/"+fileInfo.fileName();
		gzFile gzfile=gzopen((const char *)fulfile.toLocal8Bit(),"rb");
//		gzFile gzfile=gzopen("F:/access.log.20130101/access.log.20130101","rb");
		if(gzfile==NULL) {i++;continue;}

		QStringList tables;
		tables=db.tables();
		if(!tables.contains("telToCity133")||!tables.contains("telToCity153")||!tables.contains("telToCity189")) {QMessageBox::warning(NULL,"wrong data:","no city information!\nPlease read the city files first!");return;}

		char tmp[2048];
		string tmpinf;
		while(gzgets(gzfile,buf,4095)!=NULL) {
			sscanf(buf,"%[^|]|%d-%d-%d%d:%d:%d|%[^|]|%d|%[^|]|%d|%[^|]|%[^|]|%[^|]",userip,&year,&month,&date,&hourss,&minites,&seconds,tmp,&result,subpro,&bytes,telnumber,information,sources);
			sscanf(tmp+1,"%s %s",sitetype,url);
			tmpinf=information;
			if(tmpinf.find("Android")<tmpinf.length()) strcpy(os,"Android");
			else if(tmpinf.find("Linux")<tmpinf.length()) strcpy(os,"Linux");
			else if(tmpinf.find("Windows")<tmpinf.length()) strcpy(os,"Windows");
			else if(tmpinf.find("Bada")<tmpinf.length()) strcpy(os,"Bada");
			else strcpy(os,"-");
			int j;
			for(j=0;j<42;j++) {
				int pos = tmpinf.find(uachar[j].toLocal8Bit());
				if(pos<tmpinf.length() && pos!=-1) {
					int endpos = tmpinf.find("/", pos);
					strcpy(company, tmpinf.substr(pos, endpos - pos).c_str());
					break;
				}
			}
			if(j==42) strcpy(company,"-");

			int midthree=(telnumber[4]-'0')*100+(telnumber[5]-'0')*10+telnumber[6]-'0';
			QSqlQuery cityquery;
			if(telnumber[1]=='8') {
				cityquery.exec(QString("select * from telToCity133 where midthree=%1").arg(midthree));
				cityquery.next();
				citys=cityquery.value(telnumber[4]-'0').toInt()+1000;
			}
			else {
				cityquery.exec(QString("select * from telToCity189 where midthree=%1").arg(midthree));
				cityquery.next();
				citys=cityquery.value(telnumber[4]-'0').toInt();
			}

			QString s=QString("insert into totalTable values('%1',%2,%3,%4,%5,%6,%7,'%8','%9',%10,'%11',%12,'%13','%14','%15',%16,'%17')").arg(userip).arg(year).arg(month).arg(date).arg(hourss).arg(minites).arg(seconds).arg(sitetype).arg(url).arg(result).arg(subpro).arg(bytes).arg(telnumber).arg(os).arg(company).arg(citys).arg(sources);
			query.exec(s);
		}
		gzclose(gzfile);
		i++;
	}while(i<list.size());

	return ;
}

void Database::setcity()
{

}

void Database::readcityinfo(QString path)
{
	QString csv133=path+"/teltocity133.csv";
	QString csv153=path+"/teltocity153.csv";
	QString csv189=path+"/teltocity189.csv";

	char buf[1025];
	QString range;
	int tmp,count=0;
	QStringList strlst,smallrangelst;
//***************读取csv格式数据,133开头号码段***************
	memset(city,-1,sizeof(city));
	FILE *file=fopen((const char*)csv133.toLocal8Bit(),"rb");
	if(!file) {
		QMessageBox::warning(NULL,"error","can not open the csv(133) file!");
		return;
	}
	fgets(buf,1024,file);
	while(fgets(buf,1024,file)!=NULL) {
		strlst=((QString)buf).split(",",QString::KeepEmptyParts);
		for(int i=3;i<strlst.length();i++) {
			range=strlst.at(i);
			if(range[0]<'0'||range[0]>'9') continue;
			smallrangelst=range.split("*");
			for(int j=0;j<smallrangelst.length();j++) {
				if(smallrangelst.at(j).length()<=3) {
					tmp=smallrangelst.at(j).toInt();
					city[tmp][i-3]=count;
				}
				else {
					int st,end;
					sscanf((const char*)smallrangelst.at(j).toLocal8Bit(),"%d-%d",&st,&end);
					for(tmp=st;tmp<=end;tmp++) city[tmp][i-3]=count;
				}
			}
		}
		count++;
	}
	fclose(file);
/*	//写入文件
	QFile result("./telToCity133.txt");
	if(!result.open(QIODevice::WriteOnly)) return false;
	QTextStream out(&result);
	for(int i=0;i<100;i++) {
		for(int j=0;j<10;j++) out<<" "<<city[i][j];
		out<<endl;
	}
	result.close();
*/
	//建立表
	QSqlQuery query;
	query.exec(QObject::tr("create table telToCity133(midthree char(4),city0 int,city1 int,city2 int,city3 int,city4 int,city5 int,city6 int,city7 int,city8 int,city9 int)"));
/*	file=fopen("F:/serve/teltocity133.txt","rb");
	if(!file) {
		QMessageBox::warning(NULL,"error","can not open the txt file!");
		return;
	}
	int c0,c1,c2,c3,c4,c5,c6,c7,c8,c9;
	char s[1024];
	QString num;
	count=0;
	while(fgets(s,1024,file)!=NULL) {
		sscanf(s,"%d%d%d%d%d%d%d%d%d%d",&c0,&c1,&c2,&c3,&c4,&c5,&c6,&c7,&c8,&c9);
		num=QString("%1").arg(count,3,10,QChar('0'));
		query.exec(QString("insert into telToCity133 values('%1',%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)").arg(num).arg(c0).arg(c1).arg(c2).arg(c3).arg(c4).arg(c5).arg(c6).arg(c7).arg(c8).arg(c9));
		count++;
	}
	fclose(file);
*/
	QString num;
	for(int i=0;i<1000;i++) {
		num=QString("%1").arg(i,3,10,QChar('0'));
		query.exec(QString("insert into telToCity133 values('%1',%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)").arg(num).arg(city[i][0]).arg(city[i][1]).arg(city[i][2]).arg(city[i][3]).arg(city[i][4]).arg(city[i][5]).arg(city[i][6]).arg(city[i][7]).arg(city[i][8]).arg(city[i][9]));
	}
//***************读取csv格式数据,153开头号码段***************
	memset(city,-1,sizeof(city));
	strlst.clear();
	smallrangelst.clear();
	count=0;
	file=fopen((const char*)csv153.toLocal8Bit(),"rb");
	if(!file) {
		QMessageBox::warning(NULL,"error","can not open the csv(153) file!");
		return;
	}
	fgets(buf,1024,file);
	while(fgets(buf,1024,file)!=NULL) {
		strlst=((QString)buf).split(",",QString::KeepEmptyParts);
		for(int i=3;i<strlst.length();i++) {
			range=strlst.at(i);
			if(range[0]<'0'||range[0]>'9') continue;
			smallrangelst=range.split("*");
			for(int j=0;j<smallrangelst.length();j++) {
				if(smallrangelst.at(j).length()<=3) {
					tmp=smallrangelst.at(j).toInt();
					city[tmp][i-3]=count;
				}
				else {
					int st,end;
					sscanf((const char*)smallrangelst.at(j).toLocal8Bit(),"%d-%d",&st,&end);
					for(tmp=st;tmp<=end;tmp++) city[tmp][i-3]=count;
				}
			}
		}
		count++;
	}
	fclose(file);

	query.exec(QObject::tr("create table telToCity153(midthree char(4),city0 int,city1 int,city2 int,city3 int,city4 int,city5 int,city6 int,city7 int,city8 int,city9 int)"));
	for(int i=0;i<1000;i++) {
		num=QString("%1").arg(i,3,10,QChar('0'));
		query.exec(QString("insert into telToCity153 values('%1',%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)").arg(num).arg(city[i][0]).arg(city[i][1]).arg(city[i][2]).arg(city[i][3]).arg(city[i][4]).arg(city[i][5]).arg(city[i][6]).arg(city[i][7]).arg(city[i][8]).arg(city[i][9]));
	}
//***************读取csv格式数据,189开头号码段***************
	memset(city,-1,sizeof(city));
	strlst.clear();
	smallrangelst.clear();
	count=0;
	file=fopen((const char*)csv189.toLocal8Bit(),"rb");
	if(!file) {
		QMessageBox::warning(NULL,"error","can not open the csv(189) file!");
		return;
	}
	fgets(buf,1024,file);
	while(fgets(buf,1024,file)!=NULL) {
		strlst=((QString)buf).split(",",QString::KeepEmptyParts);
		for(int i=3;i<strlst.length();i++) {
			range=strlst.at(i);
			if(range[0]<'0'||range[0]>'9') continue;
			smallrangelst=range.split("*");
			for(int j=0;j<smallrangelst.length();j++) {
				if(smallrangelst.at(j).length()<=3) {
					tmp=smallrangelst.at(j).toInt();
					city[tmp][i-3]=count;
				}
				else {
					int st,end;
					sscanf((const char*)smallrangelst.at(j).toLocal8Bit(),"%d-%d",&st,&end);
					for(tmp=st;tmp<=end;tmp++) city[tmp][i-3]=count;
				}
			}
		}
		count++;
	}
	fclose(file);

	query.exec(QObject::tr("create table telToCity189(midthree char(4),city0 int,city1 int,city2 int,city3 int,city4 int,city5 int,city6 int,city7 int,city8 int,city9 int)"));
	for(int i=0;i<1000;i++) {
		num=QString("%1").arg(i,3,10,QChar('0'));
		query.exec(QString("insert into telToCity189 values('%1',%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)").arg(num).arg(city[i][0]).arg(city[i][1]).arg(city[i][2]).arg(city[i][3]).arg(city[i][4]).arg(city[i][5]).arg(city[i][6]).arg(city[i][7]).arg(city[i][8]).arg(city[i][9]));
	}

	return ;
}
