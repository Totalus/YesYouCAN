#include "widget_cantransmitter.h"

#include "document.h"
#include "application.h"

CanTransmitterWidget::CanTransmitterWidget(QWidget *parent, Document *document) : AbstractTabWidget(parent, document)
{
	m_dbc = 0;
	m_msgWidget = new CanMsgWidget(m_dbc, this);
	m_msgWidget->hide();

	m_device = 0;

	m_model = new CanTransmitterModel(this);

	m_view = new CustomTransmitTableView(this);
	//m_view->verticalHeader()->setDefaultSectionSize(18); // Row height
	m_view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClick(QPoint)));
	connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(editMessage(QModelIndex)));
	connect(m_view, SIGNAL(spacePressed(QModelIndexList)), this, SLOT(sendMessageManual(QModelIndexList)));

	m_view->setModel(m_model);

	//m_view->setColumnWidth(TX_COL_TYPE, 50);
	m_view->setColumnWidth(TX_COL_ID, 80);
	m_view->setColumnWidth(TX_COL_DATA_LEN, 50);
	m_view->setColumnWidth(TX_COL_DATA, 180);
	m_view->setColumnWidth(TX_COL_PERIOD, 80);
	m_view->setColumnWidth(TX_COL_COUNT, 50);
	m_view->setColumnWidth(TX_COL_SEND, 100);
	m_view->setColumnWidth(TX_COL_COMMENT, 300);

	//connect(m_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChangedSlot(QModelIndex)));

	//QSplitter *splitter = new QSplitter(Qt::Vertical);
	//splitter->addWidget(m_view);
	//splitter->addWidget(m_msgWidget);

	m_traceMenu = new QMenu("Trace", this);
	m_toolbar = new QToolBar(this);

	createActions();
	populateMenu();
	populateToolBar();
	createStatusBar();


	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_toolbar);
	layout->addWidget(m_view);
	layout->addWidget(m_statusBar);
	layout->setSpacing(1);
	layout->setContentsMargins(6,6,6,0);

	connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateStatusBar()));

	setLayout(layout);

	/*
	QTimer *timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()), this, SLOT(updateStatusBar()));
	timer->start(3500); // Update status bar each 3 seconds
	//*/
}

CanTransmitterWidget::~CanTransmitterWidget()
{
}

void CanTransmitterWidget::createStatusBar()
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
	connect(m_status_dbc, SIGNAL(clicked(bool)), this, SLOT(setDbc()));

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


void CanTransmitterWidget::createActions()
{
	m_associateDbcAct = new QAction(QIcon(":/icons/dbc"), "Associate DBC", this);
	connect(m_associateDbcAct, SIGNAL(triggered()), this, SLOT(setDbc()));

	m_deviceAct = new QAction(QIcon(":/icons/disconnected"), "Link to device", this);
	connect(m_deviceAct, SIGNAL(triggered()), this, SLOT(setDevice()));
}

void CanTransmitterWidget::populateMenu()
{
	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_associateDbcAct);
	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_deviceAct);
}


void CanTransmitterWidget::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;
	m_model->setDbcModel(dbc);
	m_msgWidget->setDbcModel(dbc);

	if(m_dbc == 0)
	{
		m_msgWidget->hide();
		m_view->setColumnWidth(TX_COL_ID, 100); // Size when no dbc associated
		m_status_dbc->setText("DBC: None");
		m_associateDbcAct->setIcon(QIcon(":/icons/dbc"));
	}
	else
	{
		//m_msgWidget->show();
		m_view->setColumnWidth(TX_COL_ID, 230); // Size when dbc associated
		m_status_dbc->setText("DBC : " + dbc->getFileName());
		m_associateDbcAct->setIcon(QIcon(":/icons/database_ok"));
	}
}

QMenu* CanTransmitterWidget::specificMenu()
{
	return m_traceMenu;
}

void CanTransmitterWidget::setDbc()
{
	DbcModel* dbc = DbcDialog::getDbc(Application::dbcList(), this);

	if(dbc != 0)
	{
		setDbc(dbc);
	}
}

void CanTransmitterWidget::setDevice()
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
					m_model->setDevice(m_device);
					updateStatusBar();

					m_deviceAct->setIcon(QIcon(":/icons/connected"));

					return;
				}
			}
		}
	}
}

void CanTransmitterWidget::updateStatusBar()
{
	m_status_nb_msg->setText("Message : " + QString::number(m_model->count()));


	if(m_device != 0)
		m_status_device->setText("Interface : " + m_device->getIdentifier() + " (" + QString::number(m_device->getRxRate()) + " msg/s)");
	else
		m_status_device->setText("Interface : -");

}

void CanTransmitterWidget::onRightClick(const QPoint &point)
{
	QModelIndex index = m_view->indexAt(point);

	QAction newMessageAct(QIcon(":/icons/new_message"), "New message", this);
	QAction delMessageAct(QIcon(":/icons/delete_message"), "Remove message", this);
	QAction editMessageAct(QIcon(":/icons/config_message"),"Edit message", this);

	QMenu txMenu("Messages", this);
	txMenu.addAction(&newMessageAct);
	txMenu.addAction(&delMessageAct);
	txMenu.addAction(&editMessageAct);

	if(!index.isValid())
	{
		delMessageAct.setVisible(false);
		editMessageAct.setVisible(false);
	}

	QAction *trig = txMenu.exec(m_view->mapToGlobal(point));

	if(trig == 0)
		return;
	else if(trig == &newMessageAct)
		newMessage();
	else if(trig == &delMessageAct)
		m_model->removeMessage(index.row());
	else if(trig == &editMessageAct)
	{
		editMessage(index);
	}
}

void CanTransmitterWidget::editMessage(const QModelIndex &index)
{
	if(index.row() >= m_model->count())
		return;

	MessageConstructionDialog d(m_model->getCanMessage(index.row()), m_dbc, this);

	d.exec();
}

void CanTransmitterWidget::newMessage()
{
	CanTxMessageObject *msgObj = new CanTxMessageObject;

	MessageConstructionDialog d(msgObj, m_dbc, this);

	if(d.exec() != QDialog::Accepted)
		return;

	m_model->addCanMessage(msgObj);
	connect(msgObj, SIGNAL(sendMessage(CanTraceMessage&)), this, SLOT(sendMessageToInterface(CanTraceMessage&)));
}

void CanTransmitterWidget::populateToolBar()
{
	m_toolbar->clear();

	m_toolbar->addAction(m_deviceAct);
	m_toolbar->addAction(m_associateDbcAct);
}

void CanTransmitterWidget::sendMessageManual(QModelIndexList indexes)
{
	if(m_device == 0)
		return;

	// Send all selected messages on the interface
	for(int i = 0; i < indexes.count(); i++)
	{
		// indexes contains one index for each cell

		if(indexes.at(i).column() == 0) // Do only for first column (therefore once per row)
			m_model->getCanMessage(indexes.at(i).row())->sendMessageSlot(); // Send message of corresponding index
	}
}

void CanTransmitterWidget::sendMessageToInterface(CanTraceMessage &msg)
{
	if(m_device == 0)
		return;

	m_device->writeMessage(msg);
}

