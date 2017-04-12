#include "DrawTerminal.h"
#include "data.h"

#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QMessageBox>

#include <list>

DrawTerminal::DrawTerminal(QWidget *parent /* = 0 */)
{
	totalwidth = 1024;
	totalheight = 768;
	this->setGeometry(0, 0, 1024, 768);
}

DrawTerminal::~DrawTerminal()
{

}

void DrawTerminal::paintEvent(QPaintEvent *event)
{
	QPainter painter, painterText;
//	QMessageBox::about(NULL, "hello", "here");
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(QRectF(0, 0, totalwidth, totalheight), Qt::SolidPattern);
	//Begin to draw pictures
	painter.setPen(Qt::red);
	painter.setBrush(Qt::red);

	painterText.begin(this);
	painterText.setPen(Qt::green);
	painterText.setBrush(Qt::green);
	
	int pen = 150;
	int totalnum = 0;
	for (int i = 0; i < datatime.size(); i++) {
		totalnum += datatime.at(i).num;
	}

	for (int i = 0; i < datatime.size(); i++) {
		int vnum = datacompany.at(i).size();
		painter.fillRect(QRectF(i * (60 + pen) + 60, 20, 60, vnum * 10), Qt::red);
		painterText.drawText(QRectF(i * (60 + pen) + 60, vnum * 3 + 20, 60, 60), QStringLiteral("%1-%2-%3").arg(datatime.at(i).year).arg(datatime.at(i).month).arg(datatime.at(i).date));
		
		if (i < 3) continue;
		
		list<dataCompany>::iterator newiter = newtml.at(i-3).begin();
		list<dataCompany>::iterator iter = datacompany.at(i).begin();
		for (int j = 0; j < vnum; j++) {
			painterText.drawLine(QPoint(i * (60 + pen) + 110, j * 10 + 25), QPoint(i * (60 + pen) + 120 + iter->num*pen / totalnum, j * 10 + 25));
			painterText.drawEllipse(QRectF(i * (60 + pen) + 120 + iter->num*pen/totalnum, j * 10 + 25 - 2.5, 5, 5));
			if (newiter == newtml.at(i - 3).end()) break;
			if (iter->name == newiter->name) {
//				painterText.drawLine(QPoint(i * (60 + pen) + 70, j * 10 + 25), QPoint(i * (60 + pen) + 20, j * 10 + 25));
				
				painterText.drawImage(QRectF(i * (60 + pen) + 10, j * 10, 50, 50), QImage("image/Vbubble.png"));
				painterText.drawText(QRectF(i * (60 + pen) + 10, j * 10, 50, 50), newiter->name.c_str());
				painterText.drawText(QRectF(i * (60 + pen) + 120 + iter->num*pen / totalnum + 10, j * 10 + 20, 50, 15), QString::number(newiter->num));
				newiter++;
			}
			iter++;
		}
	}
	painterText.end();
	painter.end();
}