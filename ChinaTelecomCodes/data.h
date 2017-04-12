#ifndef DATA_H
#define DATA_H
#define maxn 1000

#include <QString>
#include <QStringList>
#include <vector>
#include <List>
using namespace std;

extern int totalrecord;
extern QStringList cityname133,cityname189;
extern int numofcity133[35],numofcity189[35];
extern QStringList uaname,osname,uachar;
extern int citynum(const char *tel);
extern int citycount133[345],citycount189[345];
extern int totalcitycount133[35],totalcitycount189[35];
extern QStringList province133,province189;
extern int totalwidth,totalheight;

struct ipdata{
	int count;
	int firstnum;
	int secondnum;
};
struct urldata{
	int count;
	QString url;
};
extern ipdata ipcount[256*256];
extern urldata urlcount[1000];
extern int getnum,postnum;
extern int re200,re302,re304,reothers;
extern int yearcount[100],monthcount[12],datecount[31],hourcount[4];
extern int earlyyear,no;
extern int ua[45],os[5];
extern int telcount[3];

extern int minSup,minConf;
//Use to capture terminal
struct dataTime
{
	int year, month, date;
	long num;
};

struct dataCompany
{
	string name;
	long num;
};
//Some vars, also use to capture terminal
extern vector<dataTime> datatime;
extern vector<list<dataCompany> > datacompany;
extern vector<list<dataCompany> >newtml;
//UV-PV graphics
extern dataTime begintime, endtime;

extern void timespan();
#endif