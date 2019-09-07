#ifndef CANTRACEWIDGET_H
#define CANTRACEWIDGET_H

#include <QtWidgets>
#include <QStringList>

#include "struct_dbc.h"
#include "model_cantrace.h"
#include "pannel_canmsgdetails.h"
#include "dialog_dbcdialog.h"
#include "widget_abstracttab.h"
#include "dialog_itemdialog.h"
#include "interface_hwinterface.h"
#include "interface_wcan.h"
#include "dialog_wcandialog.h"
#include "dialog_exportdialog.h"
#include "filterexpression.h"
#include "model_customfilter.h"
#include "filterlineedit.h"
#include "widget_graphTab.h"

class Document;
class MainWindow;

class CanTraceWidget : public AbstractTabWidget
{
	Q_OBJECT

	public:
		CanTraceWidget(MainWindow *parent, Document *document);
		~CanTraceWidget();

		void setDbc(DbcModel *dbc);
		CanTraceModel* model();
		void loadTraceFile_PCAN(QString file_name);
		void loadTraceFile_BusMaster(QString file_name);
		void loadTraceFile_WiresharkExport(QString file_name);

		// From AbstractTabWidget
		virtual QMenu* specificMenu();
		virtual QToolBar* specificToolBar();

	public slots:
		void selectionChangedSlot(QModelIndex);

		void saveTrace();
		void setTraceDbc();
		void exportTraceSignals();

		void showFilterBar(bool checked);
		void applyFilter();
		void filterTextChanged(QString text);
		void showFilterHelp();

		void setDevice();

		void startStopCapture(bool checked);
		//void stopCapture();

		void updateStatusBar();

		void replayTrace(); // Replay trace on associated interface
		void sendNextMessage();

		void changeReplayIndex();

		void addGraph();

	signals:
		void dbcChanged(DbcModel *dbc); // Emitted when the dbc is updated

	private:
		void createActions();
		void populateMenu();
		void createStatusBar();
		void populateToolBar();
		void updateVisibleActions();

	private:
		DbcModel *m_dbc;
		QTableView *m_view;
		CanTraceModel *m_model;
		CanMsgWidget *m_msgWidget;
		HwInterface *m_device;
		MainWindow *m_mainWindow;

		// Menu related
		QMenu *m_traceMenu;
		QAction* m_traceDbcAct;
		QAction* m_saveTraceAct;
		QAction* m_exportSignalsAct;
		QAction* m_deviceAct;
		QAction* m_startStopCaptureAct;
		//QAction* m_stopCaptureAct;
		QAction* m_showFilterBarAct;
		QAction* m_replayTraceAct;

		QAction* m_useTimestampOffset;
		QAction* m_useMsgNumberOffset;

		QAction* m_addGraph;

		// Status bar related
		QStatusBar* m_statusBar;
		QLabel*		m_statusLabel;

		QPushButton *m_status_nb_msg;
		QPushButton *m_status_dbc;
		QPushButton *m_status_filter;
		QPushButton *m_status_device;

		// Filter related
		CustomFilterModel *m_filter_model;
		FilterLineEdit *m_filterBar;
		QToolBar* m_filterToolBar;
		QAction* m_filterHelpAct;

		// Tool bar related
		QToolBar* m_toolbar;

		QTimer* m_replayTimer;
		int m_replayIndex;
		QAction* m_replayIndexLabelAct;
};

#endif // CANTRACE_H
