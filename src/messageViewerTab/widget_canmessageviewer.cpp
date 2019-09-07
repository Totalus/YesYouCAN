#include "widget_canmessageviewer.h"

#include "document.h"
#include "application.h"

CanMessageViewerWidget::CanMessageViewerWidget(QWidget *parent, Document *document) : AbstractTabWidget(parent, document)
{
	m_dbc = 0;
	m_msgWidget = new CanMsgWidget(m_dbc, this);
	m_msgWidget->hide();

	m_device = 0;

	m_model = new CanMessageViewerModel(this);

	m_view = new QTableView(this);
	//m_view->verticalHeader()->setDefaultSectionSize(18); // Row height
	m_view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_view->setModel(m_model);

	m_view->setColumnWidth(FTRACE_COL_ID, 80);
	m_view->setColumnWidth(FTRACE_COL_TYPE, 50);
	m_view->setColumnWidth(FTRACE_COL_DATA_LEN, 40);
	m_view->setColumnWidth(FTRACE_COL_DATA, 200);
	m_view->setColumnWidth(FTRACE_COL_PERIOD, 80);
	m_view->setColumnWidth(FTRACE_COL_COUNT, 50);
	m_view->setColumnWidth(FTRACE_COL_SIGNALS, 300);

	//connect(m_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChangedSlot(QModelIndex)));

	//QSplitter *splitter = new QSplitter(Qt::Vertical);
	//splitter->addWidget(m_view);
	//splitter->addWidget(m_msgWidget);

	createMenu();
	createStatusBar();
	createToolBar();

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_toolbar);
	layout->addWidget(m_view);

	setLayout(layout);

	connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateStatusBar()));

	layout->addWidget(m_statusBar);
	layout->setSpacing(1);
	layout->setContentsMargins(6,6,6,0);

	/*
	QTimer *timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()), this, SLOT(updateStatusBar()));
	timer->start(3500); // Update status bar each 3 seconds
	//*/
}

CanMessageViewerWidget::~CanMessageViewerWidget()
{
}

void CanMessageViewerWidget::createToolBar()
{
	m_toolbar = new QToolBar(this);
	m_toolbar->addAction(m_deviceAct);
	m_toolbar->addAction(m_startStopCaptureAct);
	m_toolbar->addAction(m_resetData);
	m_toolbar->addSeparator();
	m_toolbar->addAction(m_traceDbcAct);
}

void CanMessageViewerWidget::createStatusBar()
{
	// Create status bar elements

	// Common font
	QFont font = QPushButton().font();
	font.setPointSize(8);

	// Number of messages in the trace
	m_status_nb_msg = new QPushButton(this);
	m_status_nb_msg->setFlat(true);
	m_status_nb_msg->setFont(font);

	// Related DBC
	m_status_dbc = new QPushButton("DBC : None", this);
	m_status_dbc->setFlat(true);
	m_status_dbc->setFont(font);
	connect(m_status_dbc, SIGNAL(clicked(bool)), this, SLOT(setTraceDbc()));


	m_status_device = new QPushButton("Interface : -", this);
	m_status_device->setFlat(true);
	m_status_device->setFont(font);
	connect(m_status_device, SIGNAL(clicked(bool)), this, SLOT(setDevice()));

	// Create status bar
	m_statusBar = new QStatusBar(this);
	m_statusBar->insertWidget(0, m_status_nb_msg);
	m_statusBar->insertWidget(1, m_status_device);
	m_statusBar->insertWidget(2, m_status_dbc);
	m_statusBar->setMaximumHeight(27);

	updateStatusBar();
}

void CanMessageViewerWidget::createMenu()
{
	// Actions
	m_traceDbcAct = new QAction(QIcon(":icons/dbc"), "Associate DBC", this);
	connect(m_traceDbcAct, SIGNAL(triggered()), this, SLOT(setTraceDbc()));

	m_deviceAct = new QAction(QIcon(":icons/disconnected"), "Link to device", this);
	connect(m_deviceAct, SIGNAL(triggered()), this, SLOT(setDevice()));

	m_startStopCaptureAct = new QAction(QIcon(":icons/record"), "Capture", this);
	m_startStopCaptureAct->setCheckable(true);
	m_startStopCaptureAct->setEnabled(false);
	connect(m_startStopCaptureAct, SIGNAL(triggered(bool	)), this, SLOT(startStopCapture(bool)));

	m_stopCaptureAct = new QAction("Stop Capture", this);
	connect(m_stopCaptureAct, SIGNAL(triggered()), this, SLOT(stopCapture()));

	m_resetData = new QAction(QIcon(":/icons/reset"),"Reset capture", this);
	connect(m_resetData, SIGNAL(triggered(bool)), m_model, SLOT(resetData()));

	// Menu
	m_traceMenu = new QMenu("Trace", this);
	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_traceDbcAct);
	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_deviceAct);
	m_traceMenu->addAction(m_startStopCaptureAct);
	m_traceMenu->addAction(m_stopCaptureAct);
	m_traceMenu->addAction(m_resetData);
}


CanMessageViewerModel* CanMessageViewerWidget::model()
{
	return m_model;
}

void CanMessageViewerWidget::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;
	m_model->setDbcModel(dbc);
	//m_msgWidget->setDbcModel(dbc);

	if(m_dbc == 0)
	{
		//m_msgWidget->hide();
		m_view->setColumnWidth(FTRACE_COL_ID, 100); // Size when no dbc attributed
		m_status_dbc->setText("DBC: None");

	}
	else
	{
		//m_msgWidget->show();
		m_view->setColumnWidth(FTRACE_COL_ID, 230); // Size when dbc attributed
		m_view->setColumnWidth(FTRACE_COL_SIGNALS, 350);
		m_status_dbc->setText("DBC : " + dbc->getFileName());
	}
}

/*
void CanFixedTraceWidget::selectionChangedSlot(QModelIndex current)
{
	QModelIndex index = m_filter_model->index(current.row(), FTRACE_NB_COLUMNS);

	// Update msg info in information widget
	if(current.isValid())
		m_msgWidget->setMessage(m_model->getMessage(m_filter_model->data(index,Qt::DisplayRole).toInt() - 1));

	// Update highlighting of selected object
	if(current.isValid())
		m_model->setCurrentSelectedMsgId(m_model->getMessage(m_filter_model->data(index,Qt::DisplayRole).toInt() - 1).id);
	else
		m_model->setCurrentSelectedMsgId(-1);

}
*/

QMenu* CanMessageViewerWidget::specificMenu()
{
	return m_traceMenu;
}

void CanMessageViewerWidget::setTraceDbc()
{
	DbcModel* dbc = DbcDialog::getDbc(Application::dbcList(), this);

	if(dbc != 0)
	{
		setDbc(dbc);
	}
}

void CanMessageViewerWidget::setDevice()
{
	if(m_document->isSaved())
	{
		QMessageBox::warning(this, "Saved trace", "Cannot link device to a saved trace. Create new trace to make a new capture");
		return;
	}

	QStringList iface_str;

	for(int i = 0; i < Application::interfaceList().count(); i++)
	{
		iface_str.append(Application::interfaceList().at(i)->getIdentifier() + " (" + Application::interfaceList().at(i)->getDescription() + ")");
	}

	if(iface_str.isEmpty())
	{
		QMessageBox::information(this, "No Interface", "There is no available interfaces configured. Create a new interface first to associate the interface");
		return;
	}

	ItemDialog dialog(this, "Select interface");
	dialog.setWindowTitle("Existing interfaces");
	dialog.setItemList(iface_str);
	dialog.setSelectionMode(QAbstractItemView::SingleSelection);

	if(dialog.exec() == ItemDialog::Accepted)
	{
		// Set device
		QStringList selection = dialog.getSelectedItems();

		if(!selection.isEmpty())
		{
			// Set the corresponding interface
			for(int i = 0; i < iface_str.count(); i++)
			{
				if(selection.at(0) == iface_str.at(i))
				{
					m_device = Application::interfaceList().at(i);
					m_device->getDocument()->addChildren(document()); // Add this widget document to the new device document list

					m_startStopCaptureAct->setEnabled(true);
					m_deviceAct->setIcon(QIcon(":/icons/connected"));

					updateStatusBar();
					return;
				}
			}

		}
	}
}

void CanMessageViewerWidget::startStopCapture(bool checked)
{
	if(m_device == 0)
		return;

	if(!m_device->isCapturing())
	{
		if(!m_device->setCapture(true))
		{
			// TODO : Message box explaining error
			return;
		}
	}

	if(checked)
	{
		connect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(newMessage(CanTraceMessage)), Qt::UniqueConnection);
	}
	else
	{
		disconnect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(newMessage(CanTraceMessage)));
	}

	m_deviceAct->setEnabled(!checked);

	/*
	if(m_device == 0)
		return;

	if(!m_device->isCapturing())
	{
		if(!m_device->setCapture(true))
		{
			// TODO : Message box explaining error
			return;
		}
	}

	connect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(newMessage(CanTraceMessage)), Qt::UniqueConnection);
	m_startStopCaptureAct->setEnabled(false);
	m_stopCaptureAct->setEnabled(true);
	*/
}

void CanMessageViewerWidget::stopCapture()
{
	if(m_device == 0)
		return;

	//m_device->setCapture(false); // No need to disable hardware capture

	disconnect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(newMessage(CanTraceMessage)));
	m_startStopCaptureAct->setEnabled(true);
	m_stopCaptureAct->setEnabled(false);

	updateStatusBar();
}

void CanMessageViewerWidget::updateStatusBar()
{
	m_status_nb_msg->setText("Message : " + QString::number(m_model->count()));


	if(m_device != 0)
		m_status_device->setText("Interface : " + m_device->getIdentifier() + " (" + QString::number(m_device->getRxRate()) + " msg/s)");
	else
		m_status_device->setText("Interface : -");
}

QToolBar* CanMessageViewerWidget::specificToolBar()
{
	return 0; // TODO : implement or remove
}

