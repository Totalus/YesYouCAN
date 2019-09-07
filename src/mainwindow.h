#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include "widget_dbctree.h"
#include "struct_dbc.h"
#include "widget_cantrace.h"
#include "widget_canmessageviewer.h"
#include "dialog_dbcdialog.h"
#include "dock_project.h"
#include "document.h"
#include "dialog_interfaceconfigdialog.h"
#include "dialog_pcandialog.h"
#include "widget_cantransmitter.h"
#include "widget_cansignalviewer.h"

#include "interface_pcan.h"
#include "interface_loopback.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		QList<DbcModel*> getDbcList();

	public slots:
		// Open/create new documents
		void openDbc();
		void openTrace();
		void openWatcher();
		void newTrace();
		void newFixedTrace();
		void newTransmitWindow();
		void newWatcher();
		void closeDocument(Document *doc);

		void tabChangedSlot(int index);
		void changeTabFocusOnDocument(QString specific_document_name);
		void activateDocument(Document *doc); // Document activated (take action according to document type)
		void updateTabNames();

		void newInterface();
		bool configureInterface(HwInterface *iface);

		void debug();

	protected:
		bool eventFilter(QObject *obj, QEvent *event);
		bool processRightClickOnTab(QTabWidget *obj, QMouseEvent *event);

	private:
		void createMenus();
		void createDocks();

	private:
		QTabWidget* m_tabs;
		DbcTreeWidget* m_tree;

		Document* m_root_document; // Container for all other documents
		Document* m_interface_document; // Contains all interfaces
		Document* m_orphans_document;	// Contains all orphan documents
		Document* m_dbc_document;	// Contains all DBC databases documents
		//QList<Document* > m_document_list;

		QTabWidget* m_auxiliaryTabs;

		ProjectDock* m_projectDock;

		QMenu* m_fileMenu;
		QAction* m_quitAct;

		QMenu* m_openMenu;
		QAction* m_openTraceAct;
		QAction* m_openDbcAct;
		QAction* m_openWatcherAct;

		QMenu* m_viewMenu;
		QAction* m_showProjectAct;
		QAction* m_showAuxiliaryWindowAct;

		QMenu* m_newMenu;
		QAction* m_newTraceAct;
		QAction* m_newInterfaceAct;
		QAction* m_newFixedTraceAct;
		QAction* m_newTxWidgetAct;
		QAction* m_newWatcherAct;

		Document* documentOf(QString fileName);
		Document* documentOf(AbstractTabWidget *tab);


};

#endif // MAINWINDOW_H
