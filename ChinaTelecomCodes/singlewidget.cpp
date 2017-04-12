#include "singlewidget.h"
#include <QPainter>
#include <QPaintEvent>
#include "data.h"

#include <iostream>
#include <algorithm>
using namespace std;

SingleWidget::SingleWidget(QWidget *parent,int i)
{
	num=i;
}

bool cmp(const ipdata a,const ipdata b)
{
	return a.count>b.count;
}

bool cmp2(const urldata a,const urldata b)
{
	return a.count>b.count;
}

void SingleWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(QRectF(0,0,totalwidth,totalheight),Qt::SolidPattern);

	if(num==1) {
		sort(ipcount,ipcount+256*256,cmp);
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		for(int i=0;i<30;i++) {
			painter.drawText(QRectF(50,i*15,50,100),QString("%1.%2").arg(ipcount[i].firstnum).arg(ipcount[i].secondnum));
			painter.drawText(QRectF(150,i*15,50,100),QString::number(ipcount[i].count));
			painter.drawRect(QRectF(200,i*15,200*ipcount[i].count/totalrecord,10));
		}
	}
	else if(num==2) {
		painter.setPen(Qt::yellow);
		painter.setBrush(Qt::yellow);
		painter.drawText(QRectF(0,0,50,50),QString(QStringLiteral("日期")));
		painter.drawText(QRectF(50,0,50,50),QString(QStringLiteral("访问数量")));
		for(int i=1;i<=31;i++) {
			painter.drawText(QRectF(10,i*15,50,100),QString::number(i));
			painter.drawText(QRectF(50,i*15,50,50),QString::number(datecount[i-1]));
			painter.drawRect(QRectF(100,i*15,200*datecount[i-1]/totalrecord,10));
		}
		painter.setPen(Qt::blue);
		painter.setBrush(Qt::blue);
		painter.drawText(QRectF(300,0,50,50),QString(QStringLiteral("小时")));
		painter.drawText(QRectF(350,0,50,50),QString(QStringLiteral("访问数量")));
		for(int i=1;i<=24;i++) {
			painter.drawText(QRectF(300,i*15,50,100),QString::number(i));
			painter.drawText(QRectF(350,i*15,50,50),QString::number(hourcount[i-1]));
			painter.drawRect(QRectF(400,i*15,200*hourcount[i-1]/totalrecord,10));
		}
		painter.setPen(Qt::red);
		painter.drawLine(QPoint(300,460),QPoint(500,460));
		painter.drawLine(QPoint(300,460),QPoint(300,400));
		painter.drawText(QRectF(270,460,50,50),QString(QStringLiteral("月份")));
		painter.drawText(QRectF(270,400,50,50),QString(QStringLiteral("访问数量\n百分比")));
		painter.setPen(Qt::green);
		for(int i=0;i<12;i++) {
			painter.drawText(QRectF(300+i*25,460,50,50),QString::number(i+1));
			if(i<11) painter.drawLine(QPoint(300+i*25,460-100*monthcount[i]/totalrecord),QPoint(300+(i+1)*25,460-100*monthcount[i+1]/totalrecord));
		}
	}
	else if(num==3) {
		painter.setPen(Qt::red);
		painter.drawLine(QPoint(40,450),QPoint(640,450));
		painter.drawLine(QPoint(40,450),QPoint(40,0));

		QFont font;
		font.setPixelSize(20);
		painter.setFont(font);
		int height=400/(postnum+getnum);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(Qt::yellow);
		painter.setBrush(Qt::yellow);
		painter.drawRect(QRectF(100,450-height*getnum,100,height*getnum));
		painter.drawText(QRectF(100,450,100,50),QString("GET"));
		painter.drawText(QRectF(80,420-height*getnum,140,50),QString("%1(%2%)").arg(getnum).arg(getnum*100.0/(postnum+getnum)));
		painter.drawPie(350,100,200,200,0,360*16*getnum/(postnum+getnum));
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		painter.drawRect(QRectF(250,450-height*postnum,100,height*postnum));
		painter.drawText(QRectF(250,450,100,50),QString("POST"));
		painter.drawText(QRectF(230,420-height*postnum,140,50),QString("%1(%2%)").arg(postnum).arg(postnum*100.0/(postnum+getnum)));
		painter.drawPie(350,100,200,200,360*16*getnum/(postnum+getnum),360*16*postnum/(postnum+getnum));
	}
	else if(num==4) {
		sort(urlcount,urlcount+no,cmp2);
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		for(int i=0;i<15;i++) {
			painter.drawText(QRectF(10,i*40,300,40),QString(urlcount[i].url));
			painter.drawText(QRectF(350,i*40,50,20),QString::number(urlcount[i].count));
			painter.drawRect(QRectF(400,i*40,500.0*urlcount[i].count/totalrecord,20));
		}
	}
	else if(num==5) {
		painter.setPen(Qt::red);
		painter.drawLine(QPoint(40,450),QPoint(640,450));
		painter.drawLine(QPoint(40,450),QPoint(40,0));

		painter.setBrush(Qt::magenta);
		QFont font;
		font.setPixelSize(20);
		painter.setFont(font);
		reothers=totalrecord-re302-re200-re304;
		int height=400/(totalrecord-reothers);
		painter.drawText(QRectF(0,0,100,50),QString(QStringLiteral("总记录数：%1")).arg(totalrecord));
		painter.drawRect(QRectF(50,450-height*re200,100,height*re200));
		painter.drawText(QRectF(50,450,100,50),QString("result=200"));
		painter.drawText(QRectF(30,420-height*re200,140,50),QString("%1(%2%)").arg(re200).arg(re200*100.0/totalrecord));
		painter.drawRect(QRectF(200,450-height*re302,100,height*re302));
		painter.drawText(QRectF(200,450,100,50),QString("result=302"));
		painter.drawText(QRectF(180,420-height*re302,140,50),QString("%1(%2%)").arg(re302).arg(re302*100.0/totalrecord));
		painter.drawRect(QRectF(350,450-height*re304,100,height*re304));
		painter.drawText(QRectF(350,450,100,50),QString("result=304"));
		painter.drawText(QRectF(330,420-height*re304,140,50),QString("%1(%2%)").arg(re304).arg(re304*100.0/totalrecord));
		painter.drawRect(QRectF(500,450-height*reothers,100,height*reothers));
		painter.drawText(QRectF(500,450,100,50),QString("others"));
		painter.drawText(QRectF(480,420-height*reothers,140,50),QString("%1(%2%)").arg(reothers).arg(reothers*100.0/totalrecord));
	}
	else if(num==6) {
		int tmp=telcount[0]+telcount[1]+telcount[2];
		painter.setPen(Qt::yellow);
		painter.setBrush(Qt::yellow);
		painter.drawText(QRectF(10,10,250,20),QString(QStringLiteral("以133开头的号码数量：%1，占%2%")).arg(telcount[0]).arg(telcount[0]*100.0/totalrecord));
		painter.drawPie(QRectF(150,150,200,200),0,telcount[0]*360.0/totalrecord*16);
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		painter.drawText(QRectF(10,30,250,20),QString(QStringLiteral("以153开头的号码数量：%1，占%2%")).arg(telcount[1]).arg(telcount[1]*100.0/totalrecord));
		painter.drawPie(QRectF(150,150,200,200),telcount[0]*360.0/totalrecord*16,telcount[1]*360.0/totalrecord*16);
		painter.setPen(Qt::magenta);
		painter.setBrush(Qt::magenta);
		painter.drawText(QRectF(10,50,250,20),QString(QStringLiteral("以189开头的号码数量：%1，占%2%")).arg(telcount[2]).arg(telcount[2]*100.0/totalrecord));
		painter.drawPie(QRectF(150,150,200,200),(telcount[0]+telcount[1])*360.0/totalrecord*16,telcount[2]*360.0/totalrecord*16);
		painter.setPen(Qt::blue);
		painter.setBrush(Qt::blue);
		painter.drawText(QRectF(10,70,250,20),QString(QStringLiteral("其它：%1，占%2%")).arg(totalrecord-tmp).arg((totalrecord-tmp)*100.0/totalrecord));
		painter.drawPie(QRectF(150,150,200,200),tmp*360.0/totalrecord*16,(totalrecord-tmp)*360.0/totalrecord*16);
	}
	else if(num==7) {
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		painter.drawText(QRectF(10,10,200,20),QString(QStringLiteral("操作系统统计图：（总记录数：%1）")).arg(totalrecord));
		for(int i=0;i<4;i++) {
			painter.drawText(QRectF(20,i*15+30,50,100),QString("%1:").arg(osname[i]));
			painter.drawText(QRectF(100,i*15+30,50,100),QString::number(os[i]));
			painter.drawText(QRectF(150,i*15+30,100,100),QString("%1%").arg(os[i]*100.0/totalrecord));
			painter.drawRect(QRectF(250,i*15+30,200.0*os[i]/totalrecord,10));
		}
	}
	else if(num==8) {
		painter.setPen(Qt::green);
		painter.setBrush(Qt::green);
		painter.drawText(QRectF(10,10,200,20),QString(QStringLiteral("厂商统计图：（总记录数：%1）")).arg(totalrecord));
		for(int i=0;i<30;i++) {
			painter.drawText(QRectF(10,i*15+30,60,100),QString("%1:").arg(uaname[i]));
			painter.drawText(QRectF(70,i*15+30,60,100),QString::number(ua[i]));
			painter.drawText(QRectF(130,i*15+30,70,100),QString("%1%").arg(ua[i]*100.0/totalrecord));
			painter.drawRect(QRectF(200,i*15+30,100.0*ua[i]/totalrecord,10));
		}
		for(int i=30;i<42;i++) {
			painter.drawText(QRectF(300,(i-29)*15,60,100),QString("%1:").arg(uaname[i]));
			painter.drawText(QRectF(360,(i-29)*15,60,100),QString::number(ua[i]));
			painter.drawText(QRectF(420,(i-29)*15,70,100),QString("%1%").arg(ua[i]*100.0/totalrecord));
			painter.drawRect(QRectF(490,(i-29)*15,100.0*ua[i]/totalrecord,10));
		}
	}
	else if(num==9) {
		painter.setPen(Qt::cyan);
		painter.setBrush(Qt::cyan);
		for(int i=0;i<31;i++) {
			painter.drawText(QRectF(0,0,200,50),QString(QStringLiteral("手机号数量：(总记录数：%1)")).arg(totalrecord));
			painter.drawText(QRectF(10,i*15+15,60,100),QString("%2-%1:").arg(province133[i]).arg(i));
			painter.drawText(QRectF(70,i*15+15,60,100),QString::number(totalcitycount133[i]));
			painter.drawText(QRectF(130,i*15+15,70,100),QString("%1%").arg(totalcitycount133[i]*100.0/totalrecord));
			painter.drawRect(QRectF(200,i*15+15,100.0*totalcitycount133[i]/totalrecord,10));
		}
	}
	painter.end();
}