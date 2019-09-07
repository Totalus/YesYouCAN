#ifndef SIGNALGRAPH_H
#define SIGNALGRAPH_H

#include "struct_canmessage.h"
#include "model_cantrace.h"
#include "dialog_exportdialog.h"
#include "candecoder.h"
#include "signalselectordialog.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

class SignalGraph : public QwtPlot
{
	Q_OBJECT

	public:
		SignalGraph(QWidget *parent = nullptr);
		void createMenu();
		void setDbc(DbcModel *dbc);

		void setSourceModel(CanTraceModel *model);

	protected:
		virtual void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
		virtual bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

	public slots:
		void addCurves();

	private:
		CanTraceModel *m_model;
		DbcModel *m_dbc;

		QMenu *m_menu;

		QMap<QString, QwtPlotCurve*> m_curves;

		bool m_leftMousePressed;
		QPointF m_lastMousePosition;
};

#endif // SIGNALGRAPH_H
