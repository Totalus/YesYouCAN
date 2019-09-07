#include "widget_graphTab.h"

GraphTab::GraphTab(DbcModel *dbc, QWidget* parent, Document* document, CanTraceModel *model) : AbstractTabWidget(parent, document)
{
	m_dbc = dbc;
	m_model = model;

	m_plot = new SignalGraph(this);
	m_plot->setDbc(dbc);
	m_plot->setSourceModel(m_model);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_plot);

	this->setLayout(layout);
}


void GraphTab::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;

	// Update DBC to plots
	m_plot->setDbc(dbc);
}

void GraphTab::setTraceModel(CanTraceModel *model)
{
	m_model = model;

	m_plot->setSourceModel(model);
}
