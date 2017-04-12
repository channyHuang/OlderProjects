#ifndef SHOWTABLE_H
#define SHOWTABLE_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QTableView>
#include <QtSql/QSqlTableModel>

namespace Ui
{
	class showtable;
}
class showtable : public QWidget
{
	Q_OBJECT
public:
	showtable(QWidget *parent=0);
	~showtable();

private:
	Ui::showtable *ui;

	QLabel *label,*label133,*label153,*label189;
	QTableView *tableview,*table133,*table153,*table189;
	QSqlTableModel *model,*model133,*model153,*model189;
};

#endif