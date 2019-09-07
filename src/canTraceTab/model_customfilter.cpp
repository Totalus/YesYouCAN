#include "model_customfilter.h"

CustomFilterModel::CustomFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
	m_expression.setExpression("");
	m_model = 0;
}

bool CustomFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(m_model == 0)
		return true;

	CanTraceMessage msg = m_model->getMessage(source_row);
	return m_expression.evaluate(msg);
}

bool CustomFilterModel::setExpression(QString expression, DbcModel* dbc)
{
	emit beginResetModel();
	m_expression.setExpression(expression, dbc);
	emit endResetModel();

	return m_expression.isValid();
}

void CustomFilterModel::setSourceModel(CanTraceModel *model)
{
	QSortFilterProxyModel::setSourceModel(model);
	m_model = model;
}

