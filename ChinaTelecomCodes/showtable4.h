#ifndef showtable4_H
#define showtable4_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QTableView>
#include <QtSql/QSqlTableModel>
#include <QLineEdit>

namespace Ui
{
	class showtable4;
}
class showtable4 : public QWidget
{
	Q_OBJECT
public:
	showtable4(QWidget *parent=0);
	~showtable4();

	QLineEdit *compShow;
private:
	Ui::showtable4 *ui;

	QLabel *label, *complabel;
	QTableView *tableview;
	QSqlTableModel *model;
	
};

#endif