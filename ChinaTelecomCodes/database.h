#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QString>

class Database : public QObject
{
	Q_OBJECT
public:
	Database();
	~Database();
	bool open();
	void close();
	void readloginfo(QString path);
	void readcityinfo(QString path);
	void setcity();
public:
	QSqlDatabase db;

	char userip[16];
	int year,month,date,hourss,minites,seconds;
	char sitetype[5];
	char url[2048];
	int result;
	char subpro[60];
	int bytes;
	int citys;
	char telnumber[15];
	char os[10],company[10];
	char information[2048],sources[2048];

	int city[1000][10];
};

#endif