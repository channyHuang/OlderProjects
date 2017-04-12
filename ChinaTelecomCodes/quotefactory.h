#ifndef _QUOTE_FACTORY_H_
#define _QUOTE_FACTORY_H_

#include <qwt_series_data.h>

class QuoteFactory
{
public:
    enum Stock
    {
        BMW,
		NumStocks
    };

    static QVector<QwtOHLCSample> samples2010( Stock ,int num);
    static QString title( Stock );
};

#endif
