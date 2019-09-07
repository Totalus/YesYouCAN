#ifndef CANFIXEDTRACEWIDGET_H
#define CANFIXEDTRACEWIDGET_H

#include <QtWidgets>
#include <QStringList>

#include "struct_dbc.h"
#include "model_canmessageviewer.h"
#include "pannel_canmsgdetails.h"
#include "dialog_dbcdialog.h"
#include "widget_abstracttab.h"
#include "dialog_itemdialog.h"
#include "interface_hwinterface.h"
#include "interface_wcan.h"
#include "dialog_wcandialog.h"
#include "dialog_exportdialog.h"

class Document;

class CanMessageViewerWidget : public AbstractTabWidget
{
	Q_OBJECT

	public:
		CanMessageViewerWidget(QWidget *parent, Document *document);
		~CanMessageViewerWidget();

		void setDbc(DbcModel *dbc);
		CanMessageViewerModel *model();

		// From AbstractTabWidget
		virtual QMenu* specificMenu();
		virtual QToolBar* specificToolBar();

	public slots:
		//void selectionChangedSlot(QModelIndex);

		void setTraceDbc();

		void setDevice();

		void startStopCapture(bool checked);
		void stopCapture();

		void updateStatusBar();

	private:
		void createMenu();
		void createStatusBar();
		void createToolBar();

	private:
		DbcModel *m_dbc;
		QTableView *m_view;
		CanMessageViewerModel *m_model;
		CanMsgWidget *m_msgWidget;
		HwInterface *m_device;

		// Menu related
		QMenu *m_traceMenu;
		QAction* m_traceDbcAct;
		QAction* m_deviceAct;
		QAction* m_startStopCaptureAct;
		QAction* m_stopCaptureAct;
		QAction* m_resetData;

		// Status bar related
		QStatusBar* m_statusBar;
		QLabel*		m_statusLabel;

		QPushButton *m_status_nb_msg;
		QPushButton *m_status_dbc;
		QPushButton *m_status_device;

		// Tool bar related
		QToolBar* m_toolbar;

};

#endif // CANFIXEDTRACEWIDGET_H
