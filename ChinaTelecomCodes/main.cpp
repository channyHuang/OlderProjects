#include "visualization.h"
#include <QtWidgets/QApplication>
#include "database.h"
#include <cassert>
#include <QString>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSplashScreen>

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	QSplashScreen *splash=new QSplashScreen;
	splash->setPixmap(QPixmap("./image/screen.png"));
	splash->show();

	Qt::Alignment topright=Qt::AlignRight|Qt::AlignTop;
	splash->showMessage(QObject::tr("Loading the software..."),topright,Qt::white);

	visualization w;
	w.show();
	splash->finish(&w);
	delete splash;

	return a.exec();
}
