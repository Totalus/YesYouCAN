#ifndef WIDGET_GRAPHTAB_H
#define WIDGET_GRAPHTAB_H

#include "struct_dbc.h"
#include "candecoder.h"
#include "widget_abstracttab.h"


#include "widget_signalgraph.h"


class GraphTab : public AbstractTabWidget
{
	Q_OBJECT

	public:
		GraphTab(DbcModel* dbc, QWidget *parent, Document *document, CanTraceModel *model);
		// void addSignal(QString sigName); // Add graph for that signal

	public slots:
		void setDbc(DbcModel *dbc);
		void setTraceModel(CanTraceModel *model);
		// void newMessage(CanTraceMessage &msg); // New message arrived in trace

	private:
		DbcModel *m_dbc;
		CanTraceModel *m_model;

		//CanDecoder *m_decoder;

		SignalGraph *m_plot;
};

#endif // WIDGET_GRAPHTAB_H
