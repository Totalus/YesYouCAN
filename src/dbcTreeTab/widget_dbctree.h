#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QtWidgets>
#include <QtGui>
#include "struct_dbc.h"
#include "widget_abstracttab.h"

class Document;

class DbcTreeWidget : public AbstractTabWidget
{
	Q_OBJECT

	public:
		DbcTreeWidget(QWidget* parent = 0, Document* document = 0);
		void populate(DbcModel *model);

		// From AbstractTabWidget


	public slots:
		void updateDetails(const QModelIndex &index);

	private:
		QStandardItemModel *m_model;
		QTreeView *m_view;
		QTableWidget *m_details;
		DbcModel *m_dbc;
};

#endif // TREEWIDGET_H
