#ifndef _PLOT_H_
#define _PLOT_H_

#include <qwt_plot.h>

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget * = NULL ,int num = 10);

public Q_SLOTS:
    void setMode( int );
    void exportPlot();

private Q_SLOTS:
    void showItem( QwtPlotItem *, bool on );

private:
    void populate();

	int plotnum;
};

#endif
