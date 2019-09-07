#include "mainwindow.h"

#include "application.h"

// Temporary for debug
#include "dialog_graphicsobject.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	m_tabs = new QTabWidget(this);
	m_tabs->setMovable(true);
	m_tabs->tabBar()->installEventFilter(this);
	connect(m_tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChangedSlot(int)));

	m_auxiliaryTabs = new QTabWidget(this);
	m_auxiliaryTabs->setWindowFlags(Qt::Window);
	m_auxiliaryTabs->setMovable(true);
	m_auxiliaryTabs->tabBar()->installEventFilter(this);
	m_auxiliaryTabs->hide();
	m_auxiliaryTabs->resize(800,500);

	QVBoxLayout *containerLayout = new QVBoxLayout();
	containerLayout->addWidget(m_tabs);
	containerLayout->setSpacing(1);
	containerLayout->setContentsMargins(2,2,2,2);

	QWidget *container = new QWidget(this);
	container->setLayout(containerLayout);

	m_root_document = new Document(ROOT_DOCUMENT);
	m_interface_document = new Document(ROOT_DOCUMENT, "Interfaces");
	m_orphans_document = new Document(ROOT_DOCUMENT, "Orphans");
	m_dbc_document = new Document(ROOT_DOCUMENT, "Databases");

	m_root_document->addChildren(m_interface_document);
	m_root_document->addChildren(m_orphans_document);
	m_root_document->addChildren(m_dbc_document);

	setCentralWidget(container);
	resize(1000,600);

	createDocks();
	createMenus();

	connect(m_root_document, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));
}

MainWindow::~MainWindow()
{
	delete m_root_document;
}


void MainWindow::createDocks()
{
	m_projectDock = new ProjectDock(this, m_root_document);
	m_projectDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	addDockWidget(Qt::LeftDockWidgetArea, m_projectDock);
	//connect(m_projectDock, SIGNAL(documentSelectionChanged(QString)), this, SLOT(changeTabFocusOnDocument(QString)));
	connect(m_projectDock, SIGNAL(documentActivated(Document*)), this, SLOT(activateDocument(Document*)));
}


void MainWindow::createMenus()
{
	// Actions
	m_quitAct = new QAction("Quit",this);
	connect(m_quitAct,SIGNAL(triggered()), qApp, SLOT(quit()));

	m_openDbcAct = new QAction(QIcon(":/icons/dbc"), "CAN Database", this);
	connect(m_openDbcAct, SIGNAL(triggered()), this, SLOT(openDbc()));

	m_openTraceAct = new QAction(QIcon(":/icons/trace"), "CAN Trace", this);
	connect(m_openTraceAct, SIGNAL(triggered(bool)), this, SLOT(openTrace()));

	m_openWatcherAct = new QAction(QIcon(":/icons/eye"), "CAN Signal Dashboard", this);
	connect(m_openWatcherAct, SIGNAL(triggered(bool)), this, SLOT(openWatcher()));

	m_showProjectAct = new QAction("Document pannel", this);
	m_showProjectAct->setCheckable(true);
	m_showProjectAct->setChecked(true);
	connect(m_showProjectAct, SIGNAL(toggled(bool)), m_projectDock, SLOT(setVisible(bool)));
	connect(m_projectDock, SIGNAL(visibilityChanged(bool)), m_showProjectAct, SLOT(setChecked(bool)));

	m_showAuxiliaryWindowAct = new QAction(QIcon(":/icons/auxiliary_window"), "Auxiliary window", this);
	connect(m_showAuxiliaryWindowAct, SIGNAL(triggered()), m_auxiliaryTabs, SLOT(show()));

	m_newTraceAct = new QAction(QIcon(":/icons/trace"), "CAN Trace", this);
	connect(m_newTraceAct, SIGNAL(triggered()), this, SLOT(newTrace()));

	m_newInterfaceAct = new QAction(QIcon(":/icons/interface"), "CAN Interface", this);
	connect(m_newInterfaceAct, SIGNAL(triggered()), this, SLOT(newInterface()));

	m_newFixedTraceAct = new QAction(QIcon(":/icons/receive"), "CAN Messages Viewer", this);
	connect(m_newFixedTraceAct, SIGNAL(triggered()),this, SLOT(newFixedTrace()));

	m_newTxWidgetAct = new QAction(QIcon(":/icons/transmit"), "CAN Message Sender", this);
	connect(m_newTxWidgetAct, SIGNAL(triggered(bool)), this, SLOT(newTransmitWindow()));

	m_newWatcherAct = new QAction(QIcon(":/icons/eye"), "CAN Signal Dashboard", this);
	connect(m_newWatcherAct, SIGNAL(triggered(bool)), this, SLOT(newWatcher()));

	// Menus
	m_fileMenu = new QMenu("File", this);
	m_fileMenu->addAction(m_quitAct);

	m_openMenu = new QMenu("Open", this);
	m_openMenu->addAction(m_openDbcAct);
	m_openMenu->addAction(m_openTraceAct);
	m_openMenu->addAction(m_openWatcherAct);

	m_viewMenu = new QMenu("View", this);
	m_viewMenu->addAction(m_showProjectAct);
	m_viewMenu->addAction(m_showAuxiliaryWindowAct);

	m_newMenu = new QMenu("New", this);
	m_newMenu->addAction(m_newInterfaceAct);
	m_newMenu->addSeparator();
	m_newMenu->addAction(m_newTraceAct);
	m_newMenu->addAction(m_newFixedTraceAct);
	m_newMenu->addAction(m_newTxWidgetAct);
	m_newMenu->addAction(m_newWatcherAct);

	// QAction* debugAct = new QAction("Debug", this);
	// connect(debugAct, SIGNAL(triggered(bool)), this, SLOT(debug()));

	// Menu bar
	menuBar()->addMenu(m_fileMenu); // Always visible
	menuBar()->addMenu(m_openMenu); // Always visible
	menuBar()->addMenu(m_newMenu);	// Always visible
	menuBar()->addMenu(m_viewMenu); // Always visible
	// menuBar()->addAction(debugAct);
}


void MainWindow::openDbc()
{

	// Get the file name
	QStringList file_list = QFileDialog::getOpenFileNames(this,QString(),QString(),"PEAK CAN database(*.dbc)");

	for(int i = 0; i < file_list.length(); i++)
	{
		Document *doc = 0;
		DbcModel *dbc = 0;

		// Check if dbc structure is already loaded
		for (int j = 0; j < Application::dbcList().length(); j++)
		{
			if (Application::dbcList().at(j)->getFullPath() == file_list.at(i))
			{
				dbc = Application::dbcList().at(j);
				break;
			}
		}

		if(dbc == 0)
		{
			// Load DBC file
			dbc = new DbcModel(file_list.at(i));
			QStringList parsing_errors = dbc->getErrors();
			Application::dbcList().append(dbc);

			if (!parsing_errors.isEmpty())
			{
				QMessageBox::warning(this,"Parsing error(s)", parsing_errors.join('\n'));
			}
		}

		// Check if document for this model exists
		QList<Document*> document_list = m_root_document->getAllChildrenRecursive();

		for(int j = 0; j < document_list.count(); j++)
		{
			if( (document_list.at(j)->getType() == CAN_DBC) && (document_list.at(j)->filePath() == dbc->getFullPath()) )
			{
				// Document matches
				doc = document_list.at(j); // Get document pointer

				if(doc->isOpened())
				{
					m_tabs->setCurrentWidget(doc->getViewWidget()); // Set focus on document
					break;
				}
				else
					break; // Document exists but not opened
			}
		}

		if(doc == 0)
		{
			doc = new Document(CAN_DBC, file_list.at(i), &Application::dbcList(), this);
			connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

			m_dbc_document->addChildren(doc);
		}

		if(doc->getViewWidget() == 0)
		{
			DbcTreeWidget* tree = new DbcTreeWidget(this, doc);
			tree->populate(dbc);
			doc->setViewWidget(tree);
		}

		if(!doc->isOpened())
		{
			m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), file_list.at(i).split('/').last().split('\\').last());
			m_tabs->setCurrentIndex(m_tabs->count() - 1);
			doc->setOpened(true);
		}
	}

	//m_projectDock->update();
}

void MainWindow::openTrace()
{
	// Get the files names
	QStringList file_list = QFileDialog::getOpenFileNames(this,QString(),QString(),"PEAK trace (*.trc);;BusMaster Trace(*.log)");

	for(int i = 0; i < file_list.count(); i++) // For each file in list
	{
		Document *doc = 0;

		// Check if already part of loaded / opened documents
		QList<Document*> document_list = m_root_document->getAllChildrenRecursive();
		for(int j = 0; j < document_list.count(); j++)
		{
			if(document_list.at(j)->filePath() == file_list.at(i)) // File path match
			{
				doc = document_list.at(j); // Get document object
				break;
			}
		}

		if(doc == 0) // Document is not already in the project
		{
			doc = new Document(CAN_TRACE, file_list.at(i), &Application::dbcList(), this); // Create document object
			connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));
		}

		//m_document_list.append(doc); // Add document to the document list
		m_orphans_document->addChildren(doc);


		if(!doc->isOpened())
		{
			FILE_TYPE_t trace_type = doc->getFileType();

			if(trace_type == CAN_TRACE_BUSMASTER)
			{
				// Create view widget
				CanTraceWidget* trace_widget = new CanTraceWidget(this, doc);
				trace_widget->loadTraceFile_BusMaster(doc->filePath());
				connect(trace_widget, SIGNAL(openDocumentRequest(Document*)), this, SLOT(openDocument(Document*)));

				doc->setViewWidget(trace_widget);
				doc->setOpened(true);
			}
			else if(trace_type == CAN_TRACE_PEAK)
			{
				CanTraceWidget* trace_widget = new CanTraceWidget(this, doc);
				trace_widget->loadTraceFile_PCAN(doc->filePath());
				connect(trace_widget, SIGNAL(openDocumentRequest(Document*)), this, SLOT(openDocument(Document*)));

				doc->setViewWidget(trace_widget);
				doc->setOpened(true);
			}
			m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Add view to tabs
		}

		// Set focus to the document
		changeTabFocusOnDocument(doc->fileName());
	}

	//m_projectDock->update();
}

void MainWindow::newTrace()
{
	// Find an unexisting name for the trace
	QString traceName;
	for(int i = 0; i < 100; i++)
	{
		traceName = "Trace" + QString::number(i);

		if(documentOf(traceName) == 0)
			break;
	}

	// Create the document object for this document
	Document *doc = new Document(CAN_TRACE, traceName, &Application::dbcList(), this);
	connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

	CanTraceWidget* trace_widget = new CanTraceWidget(this, doc);
	doc->setViewWidget(trace_widget);
	doc->setSaved(false);
	doc->setExists(false);
	doc->setOpened(true);

	// Add widget to new tab
	m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Widget should exist
	m_tabs->setCurrentIndex(m_tabs->count() - 1);

	//m_document_list.append(doc);
	m_orphans_document->addChildren(doc);

	trace_widget->setDevice(); // Select a device

	//m_projectDock->update();
}

void MainWindow::tabChangedSlot(int index)
{
	static QToolBar* currentToolBar = 0;

	if(index == -1)
	{
		removeToolBar(0); // Remove tool bar
		return;
	}

	if(currentToolBar != 0)
		removeToolBar(currentToolBar);

	currentToolBar = qobject_cast<AbstractTabWidget*>(m_tabs->widget(index))->specificToolBar();

	// Set specific tool bar
	if(currentToolBar != 0)
		addToolBar(currentToolBar);

	//setToolBar(qobject_cast<AbstractTabWidget*>(m_tabs->widget(index))->specificToolBar());
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == m_tabs->tabBar() || obj == m_auxiliaryTabs->tabBar())
	{

		if (event->type() == QEvent::MouseButtonRelease)
		{

			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

			if(mouseEvent->button() == Qt::RightButton)
			{
				return processRightClickOnTab(qobject_cast<QTabWidget*>(obj->parent()), mouseEvent);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
			// pass the event on to the parent class
			return QMainWindow::eventFilter(obj, event);
	}
}


bool MainWindow::processRightClickOnTab(QTabWidget *tabs, QMouseEvent *mouseEvent)
{
	// Compute the tab number
	int c = tabs->tabBar()->count();
	int tabIndex = -1;
	QPoint position = mouseEvent->pos();

	for (int i=0; i < c; i++)
	{
			if (tabs->tabBar()->tabRect(i).contains(position))
			{
					tabIndex = i;
					break;
			}
	}

	if (tabIndex == -1) // In case the tab number was not computed successfully
			return QObject::eventFilter(tabs, mouseEvent);

	tabs->setCurrentIndex(tabIndex);

	// Create menu

	QMenu menu;
	QList<QAction *> action_list;

	QAction closeTab(QIcon(":/icons/close"),"Close", &menu);
	action_list.append(&closeTab);

	QAction separateTab(QIcon(":/icons/auxiliary_window"),"Move to auxiliary window", &menu);
	QAction mergeTab(QIcon(":icons/auxiliary_window"), "Bring tab to main window", &menu);

	if (tabs == m_tabs) // Main window tabs
		action_list.append(&separateTab);
	else if (tabs == m_auxiliaryTabs) // Auxiliary window tabs
		action_list.append(&mergeTab);

	menu.addActions(action_list);

	// Add specific menu actions for this tab

	AbstractTabWidget *tab = qobject_cast<AbstractTabWidget*>(tabs->widget(tabIndex));
	QMenu *specificMenu = tab->specificMenu();

	if(specificMenu != 0)
	{
		menu.addSeparator();
		menu.addActions(specificMenu->actions());
	}


	QAction* actionSelected = menu.exec(QCursor::pos()); // Execute menu

	if(actionSelected == 0)
		return true; // Do nothing


	if(actionSelected == &closeTab)
	{
		// Close document
		if(tab->document() != 0)
			closeDocument(tab->document());

	}
	else if (actionSelected == &separateTab)
	{
		// Bring tab to auxiliary window

		QString title = tabs->tabText(tabIndex);

		m_tabs->removeTab(tabIndex);
		m_auxiliaryTabs->addTab(tab, title);

		m_auxiliaryTabs->show();
	}
	else if (actionSelected == &mergeTab)
	{
		// Bring tab to main window

		QString title = tabs->tabText(tabIndex);

		m_auxiliaryTabs->removeTab(tabIndex);
		m_tabs->addTab(tab, tab->document()->getIcon(), title);
		m_tabs->setCurrentIndex(m_tabs->count() - 1);


		if (m_auxiliaryTabs->count() == 0)
			m_auxiliaryTabs->hide();
	}


	return true;

}

void MainWindow::changeTabFocusOnDocument(QString specific_document_name)
{
	for(int i = 0; i < m_tabs->count(); i++)
	{
		if(specific_document_name == m_tabs->tabText(i))
		{
			m_tabs->setCurrentIndex(i);
			break;
		}
	}

	for(int i = 0; i < m_auxiliaryTabs->count(); i++)
	{
		if(specific_document_name == m_auxiliaryTabs->tabText(i))
		{
			m_auxiliaryTabs->setCurrentIndex(i);
			m_auxiliaryTabs->show();
			break;
		}
	}
}


QList<DbcModel*> MainWindow::getDbcList()
{
	return Application::dbcList();
}
Document* MainWindow::documentOf(AbstractTabWidget *tab)
{
	QList<Document*> document_list = m_root_document->getAllChildrenRecursive();

	for(int i = 0; i < document_list.count(); i++)
	{
		if(document_list.at(i)->getViewWidget() == tab)
			return document_list.at(i);
	}

	return 0;
}

Document* MainWindow::documentOf(QString fileName)
{
	QList<Document*> document_list = m_root_document->getAllChildrenRecursive();

	for(int i = 0; i < document_list.count(); i++)
	{
		if(document_list.at(i)->fileName() == fileName)
			return document_list.at(i);
	}

	return 0;
}

void MainWindow::closeDocument(Document *doc)
{
	//*
	if(doc->hasChildren())
	{
		if(QMessageBox::question(this, "Children documents", "All the related document to this document will be delteded. Continue ?") == QMessageBox::No)
			return;
	}
	//*/

	// First close all children documents
	while(!doc->getChildren().empty())
	{
		Document* child = doc->getChildren().first();
		closeDocument(child); // Recursive call
		doc->removeChild(child);
	}

	// Retreive view widget if exists
	AbstractTabWidget* widget = doc->getViewWidget();

	if(widget != 0)
	{
		// Remove widget from tabs
		int index = m_tabs->indexOf(widget);
		if(index != -1)
			m_tabs->removeTab(index);

		index = m_auxiliaryTabs->indexOf(widget);
		if(index != -1)
			m_auxiliaryTabs->removeTab(index);

		// Delete widget
		delete widget;
		widget = 0;
		doc->setViewWidget(0);
		doc->setOpened(false);
	}

	// Remove document from document list
	//m_document_list.removeOne(doc);

	// Delete document object
	delete doc;	// Will remove from its parent when destroying
	doc = 0;

	m_projectDock->update();
}


void MainWindow::newInterface()
{
	QStringList interfaceList;
	interfaceList << "Loopback (virtual interface)";
	interfaceList << "PCAN-USB (PEAK)";

	ItemDialog dialog(this, "Interface type");
	dialog.setWindowTitle("Device Type");
	dialog.setItemList(interfaceList);
	dialog.setSelectionMode(QAbstractItemView::SingleSelection);
	dialog.resize(400, dialog.height());

	if(dialog.exec() != QDialog::Accepted)
		return;

	QStringList selection = dialog.getSelectedItems();

	HwInterface *iface = 0;
	if(selection.count() > 0)
	{
		if(selection.at(0) == interfaceList.at(0)) // Loopback interface
		{
			iface = new LoopbackInterface(this);
		}

		else if (selection.at(0) == interfaceList.at(1)) // PCAN interface
		{
			iface = new PCanInterface(this);
		}
	}

	if(iface == 0)
		return;

	//connect(iface->getDocument(), SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

	if(configureInterface(iface))
	{
		// Check if a device is already configured the same
		bool exists = false;
		for(int i = 0; i < Application::interfaceList().count(); i++)
		{
			if(iface->getDescription() == Application::interfaceList().at(i)->getDescription())
			{
				exists = true;
				break;
			}
		}

		if(exists)
		{
			QMessageBox::warning(this, "Interface exists", "An interface already exists with the same configuration. The interface won't be created.");
			delete iface;
			return;
		}
		else
		{
			iface->initialize(); // Initialize device (if possible)
			Application::interfaceList().append(iface);
			m_interface_document->addChildren(iface->getDocument());
			Document* doc = iface->getDocument();
			doc->setDocumentName(doc->getHwInterface()->getIdentifier() + " (" + doc->getHwInterface()->interfaceTypeStr() + ")");
			connect(iface->getDocument(), SIGNAL(documentChanged()), this, SLOT(updateTabNames()));
			updateTabNames(); // Update
			//m_projectDock->update();
		}
	}
	else
	{
		delete iface;
	}

}

bool MainWindow::configureInterface(HwInterface *iface)
{
	if(iface->interfaceType() == INTERFACE_PCAN)
	{		
		//InterfaceConfigDialog d(iface->interfaceType(), this);
		//d.setName(iface->getIdentifier());

		// Set entry data
		PCanInterface *pcan = qobject_cast<PCanInterface*>(iface);

		PCanDialog d(pcan, this);
		if(d.exec() != QDialog::Accepted)
			return false;

		/*
		d.setSupportedBitrate(pcan->getSupportedBitrates());
		d.setBitrate(pcan->getBitRate());
		d.setChannelList(pcan->getAvailableDeviceList());

		if(d.exec() != QDialog::Accepted)
			return false;

		pcan->setIdentifier(d.getName());
		pcan->setBitRate(d.getBitrate());
		pcan->setChannel(d.getChannel());
		//*/
	}

	else if(iface->interfaceType() == INTERFACE_LOOPBACK)
	{
		return true;
	}

	//m_projectDock->update();

	return true;

}


void MainWindow::newFixedTrace()
{
	// Find an unexisting name for the trace
	QString traceName;
	for(int i = 0; i < 100; i++)
	{
		traceName = "Messages" + QString::number(i);

		if(documentOf(traceName) == 0)
			break;
	}

	// Create the document object for this document
	Document *doc = new Document(CAN_FIXED_TRACE, traceName, &Application::dbcList(), this);
	connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

	CanMessageViewerWidget* trace_widget = new CanMessageViewerWidget(this, doc);
	doc->setViewWidget(trace_widget);
	doc->setSaved(false);
	doc->setExists(false);

	// Add widget to new tab
	m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Widget should exist
	m_tabs->setCurrentIndex(m_tabs->count() - 1);

	//m_document_list.append(doc);
	m_orphans_document->addChildren(doc);

	trace_widget->setDevice();

	//m_projectDock->update();

}

void MainWindow::newTransmitWindow()
{
	// Find an unexisting name for the trace
	QString docName;
	for(int i = 0; i < 100; i++)
	{
		docName = "Transmit" + QString::number(i);

		if(documentOf(docName) == 0)
			break;
	}

	// Create the document object for this document
	Document *doc = new Document(CAN_TRANSMIT, docName, &Application::dbcList(), this);
	connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

	CanTransmitterWidget* transmit_widget = new CanTransmitterWidget(this, doc);
	doc->setViewWidget(transmit_widget);
	doc->setSaved(false);
	doc->setExists(false);

	// Add widget to new tab
	m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Widget should exist
	m_tabs->setCurrentIndex(m_tabs->count() - 1);

	//m_document_list.append(doc);
	m_orphans_document->addChildren(doc);

	//m_projectDock->update();
}



// This function is just a useful shortcut to test things during development
void MainWindow::debug()
{
	qDebug() << "debug() was called";
}

void MainWindow::newWatcher()
{
	// Find an unexisting name for the trace
	QString docName;
	for(int i = 0; i < 100; i++)
	{
		docName = "Dashboard" + QString::number(i);

		if(documentOf(docName) == 0)
			break;
	}

	// Create the document object for this document
	Document *doc = new Document(CAN_WATCHER, docName, &Application::dbcList(), this);
	connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));

	CanSignalViewerWidget* watcher_widget = new CanSignalViewerWidget(this, doc);

	doc->setViewWidget(watcher_widget);
	doc->setSaved(false);
	doc->setExists(false);
	doc->setOpened(true);

	// Add widget to new tab
	m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Widget should exist
	m_tabs->setCurrentIndex(m_tabs->count() - 1);

	m_orphans_document->addChildren(doc);
}

void MainWindow::openWatcher()
{
	// Get the files names
	QStringList file_list = QFileDialog::getOpenFileNames(this,QString(),QString(),"Dashboard Layout (*.wat)");

	for(int i = 0; i < file_list.count(); i++) // For each file in list
	{
		Document *doc = m_root_document->getChildren(file_list.at(i));

		/*// Check if already part of loaded / opened documents
		QList<Document*> document_list = m_root_document->getAllChildrenRecursive();
		for(int j = 0; j < document_list.count(); j++)
		{
			if(document_list.at(j)->filePath() == file_list.at(i)) // File path match
			{
				doc = document_list.at(j); // Get document object
				break;
			}
		}*/

		if(doc == 0) // Document is not already in the project
		{
			doc = new Document(CAN_WATCHER, file_list.at(i), &Application::dbcList(), this); // Create document object
			connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));
		}

		m_orphans_document->addChildren(doc);

		if(!doc->isOpened())
		{
			CanSignalViewerWidget* watcher_widget = new CanSignalViewerWidget(this, doc);
			watcher_widget->openLayout(doc->filePath());

			doc->setViewWidget(watcher_widget);
			doc->setOpened(true);

			m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Add view to tabs
		}

		// Set focus to the document
		changeTabFocusOnDocument(doc->fileName());
	}
}

void MainWindow::updateTabNames()
{
	for(int i = 0; i < m_tabs->count(); i++)
	{
		AbstractTabWidget *w = qobject_cast<AbstractTabWidget*>(m_tabs->widget(i));

		if(w != NULL) // Cast successful
			m_tabs->setTabText(i, w->document()->fileName());
	}

	m_projectDock->update(); // Also update project dock
}


void MainWindow::activateDocument(Document *doc)
{
	// TODO : utiliser doc->isOpened() éventuellement
	if(doc->getViewWidget() != 0)
	{
		m_tabs->setCurrentWidget(doc->getViewWidget());
	}
	else
	{
		if(doc->getType() == CAN_INTERFACE)
		{
			// Open CAN interface configuration window (dialog)
		}
		else if(doc->getType() == CAN_DBC)
		{
			// Open DBC in a tab
		}
	}
}

QTabWidget* MainWindow::getTabWidget()
{
	return m_tabs;
}

/*
void MainWindow::addGraph(Document *parent_doc)
{
	if(parent_doc == 0)
		return;

	// Find an unexisting name for the trace
	QString docName;
	for(int i = 0; i < 100; i++)
	{
		docName = parent_doc->documentName() + ":Graph" + QString::number(i);

		if(documentOf(docName) == 0)
			break;
	}

	// Create the document object for this document
	Document *doc = new Document(CAN_FIXED_TRACE, docName, &m_dbc_list, this);
	connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTabNames()));


	GraphTab *graph_tab = new GraphTab(0, this, doc);
	doc->setViewWidget(graph_tab);
	doc->setSaved(false);
	doc->setExists(false);

	// Add widget to new tab
	m_tabs->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName()); // Widget should exist
	m_tabs->setCurrentIndex(m_tabs->count() - 1);

	trace_widget->setDevice();

	AbstractTabWidget *w = doc->getViewWidget();
	if(w != 0)
	{
		m_tabs->addTab(w, doc->getIcon(), doc->documentName());
	}
}
	*/
