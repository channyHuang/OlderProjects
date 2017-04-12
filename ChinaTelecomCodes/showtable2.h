#ifndef showtable2_H
#define showtable2_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QTableView>
#include <QtSql/QSqlTableModel>

namespace Ui
{
	class showtable2;
}
class showtable2 : public QWidget
{
	Q_OBJECT
public:
	showtable2(QWidget *parent=0);
	~showtable2();

private:
	Ui::showtable2 *ui;

	QLabel *label,*label133,*label153,*label189;
	QTableView *tableview,*table133,*table153,*table189;
	QSqlTableModel *model,*model133,*model153,*model189;
};

#endif