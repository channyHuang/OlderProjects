#include "quotefactory.h"
#include <qwt_date.h>

#include <QFile>
#include <QMessageBox>
#include "database.h"

typedef struct
{
    int day;

    double high;

} t_Data2010;

static t_Data2010 bmwData[365] =
{
	{ 1, 0}
};

QVector<QwtOHLCSample> QuoteFactory::samples2010( Stock stock ,int num)
{
//计数
	QFile *databasefile = new QFile;
	if (!databasefile->exists("./database.db")) { QMessageBox::warning(NULL, "error!", "Please read the log files first!"); }
	Database db;
	db.open();
	QStringList tables;
	tables = db.db.tables();
	if (!tables.contains("totalTable")) { QMessageBox::warning(NULL, "wrong data:", "no table totalTable"); }
	QSqlQuery query;
	int i;

	switch (num) {
	case 11:
		i = 0;	
		query.exec("select months, dates, hourss, count(*) from totalTable group by months, dates, hourss");
		while (query.next()) {
			bmwData[i] = { i + 1, query.value(2).toInt() };
			i++;
		}
		break;
//以后添加
	case 10:
		break;
	default:
		break;
	}
	

	db.close();

    const t_Data2010 *data = NULL;
    int numSamples = 0;

    switch( stock )
    {
        case BMW:
        {
            data = bmwData;
            numSamples = sizeof( bmwData ) / sizeof( t_Data2010 );
            break;
        }
        default:
            break;
    }

    QVector<QwtOHLCSample> samples;
    samples.reserve( numSamples );

    QDateTime year2010( QDate( 2010, 1, 1 ), QTime( 0, 0 ), Qt::UTC );

    for ( int i = 0; i < numSamples; i++ )
    {
        const t_Data2010 &ohlc = data[ i ];

        samples += QwtOHLCSample( 
            QwtDate::toDouble( year2010.addDays( ohlc.day ) ), ohlc.high);
    }

    return samples;
}

QString QuoteFactory::title( Stock stock )
{
    switch( stock )
    {
        case BMW:
            return "BMW";
        default:
            break;
    }

    return "Unknown";
}
