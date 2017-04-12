#ifndef DRAWTERMINAL_H
#define DRAWTERMINAL_H

#include <QWidget>

class DrawTerminal : public QWidget
{
	Q_OBJECT
public:
	DrawTerminal(QWidget *parent = 0);
	~DrawTerminal();
protected:
	void paintEvent(QPaintEvent *event);
private:
	int totalwidth, totalheight;
};

#endif