#ifndef CITYDISTWINDOW_H
#define CITYDISTWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "plot.h"

class cityDistWindow : public QMainWindow
{
public:
	cityDistWindow(QWidget * = NULL, int num = 10);
private:
	Plot *d_plot;
};

#endif