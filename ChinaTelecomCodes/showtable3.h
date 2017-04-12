#ifndef showtable3_H
#define showtable3_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QTableView>
#include <QtSql/QSqlTableModel>

namespace Ui
{
	class showtable3;
}
class showtable3 : public QWidget
{
	Q_OBJECT
public:
	showtable3(QWidget *parent=0);
	~showtable3();

private:
	Ui::showtable3 *ui;

	QLabel *label;
	QTableView *tableview;
	QSqlTableModel *model;
};

#endif