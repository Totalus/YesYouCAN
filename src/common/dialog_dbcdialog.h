#ifndef DBCDIALOG_H
#define DBCDIALOG_H

#include <QtWidgets>
#include "struct_dbc.h"

class DbcDialog : public QDialog
{
	Q_OBJECT

	public:
		DbcDialog(QList<DbcModel *> dbc_list, QWidget * parent = 0, Qt::WindowFlags f = 0);
		QString dbcPath();
		DbcModel* dbc();

		static QString getDbcPath(QList<DbcModel *> dbc_list, QWidget * parent = 0);
		static DbcModel* getDbc(QList<DbcModel *> dbc_list, QWidget * parent = 0);

	private:
		QListWidget* m_listWidget;
		QList<DbcModel*> m_dbc_list;


};

#endif // DBCDIALOG_H
