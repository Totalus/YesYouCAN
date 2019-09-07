#ifndef CANMSGWIDGET_H
#define CANMSGWIDGET_H

#include <QtWidgets>
#include "struct_dbc.h"
#include "model_cantrace.h"



class CanMsgWidget : public QWidget
{
	public:
		CanMsgWidget(DbcModel *dbc, QWidget* parent = 0);
		void setDbcModel(DbcModel* dbc);
		void setMessage(const CanTraceMessage &msg);

	private:
		QTreeView *m_tree;
		QStandardItemModel *m_model;
		QStandardItem *m_idItem;
		QStandardItem *m_signalsItem;
		QStandardItem *m_comment;

		DbcModel *m_dbc;
};

#endif // CANMSGDETAILSWIDGET_H
