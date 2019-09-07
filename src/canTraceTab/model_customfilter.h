#ifndef CUSTOMFILTERMODEL_H
#define CUSTOMFILTERMODEL_H


#include <QtWidgets>
#include "filterexpression.h"

class CustomFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		CustomFilterModel(QObject *parent = 0);
		bool setExpression(QString expression, DbcModel *dbc);
		void setSourceModel(CanTraceModel *model);
		FilterExpression getExpression();

	protected:
		bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

	private:
		FilterExpression m_expression;

		CanTraceModel* m_model;

};

#endif // CUSTOMFILTERMODEL_H
