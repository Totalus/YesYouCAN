#include "widget_dbctree.h"

DbcTreeWidget::DbcTreeWidget(QWidget* parent, Document *document) : AbstractTabWidget(parent, document)
{
	m_model = new QStandardItemModel(this);
	m_model->setHorizontalHeaderLabels(QStringList() << "Messages");

	m_view = new QTreeView(this);
	m_view->setModel(m_model);

	m_details = new QTableWidget(10,2,this);
	m_details->verticalHeader()->hide();
	m_details->setHorizontalHeaderLabels(QStringList() << "Parameter" << "Value");
	m_details->horizontalHeader()->setStretchLastSection(true);
	m_details->setColumnWidth(0,150);

	// Fill the table widget with empty items (to be filled later)
	for(int i = 0; i < m_details->rowCount(); i++)
		for(int j = 0; j < m_details->columnCount(); j++)
		{
			m_details->setItem(i,j, new QTableWidgetItem());
			m_details->setRowHidden(i, true);
		}


	QObject::connect(m_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(updateDetails(QModelIndex)));

	QSplitter *splitter = new QSplitter(this);
	splitter->addWidget(m_view);
	splitter->addWidget(m_details);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(splitter);

	setLayout(layout);
}


void DbcTreeWidget::populate(DbcModel *model)
{
	QList<quint32> id_list = model->getMessagesId();

	for (int i = 0; i < id_list.length(); i++)
	{
		CanMessage* msg = model->getMessage(id_list.at(i));

		QStandardItem* msg_item = new QStandardItem(msg->getName()); // Message item

		QList< QPair<CanSignal*, quint8> > sig_list = msg->getSignals();

		for (int j = 0; j < sig_list.length(); j++)
		{
			QStandardItem* sig_item = new QStandardItem(sig_list.at(j).first->getName()); // Signal item
			msg_item->appendRow(sig_item);
		}

		m_model->appendRow(msg_item);
	}

	m_dbc = model;

}


void DbcTreeWidget::updateDetails(const QModelIndex &index)
{
	QList< QPair<QString, QString> > param_list;

	if(m_model->itemFromIndex(index)->parent() == 0) // It is a message
	{
		CanMessage *msg = m_dbc->getMessage(m_model->itemFromIndex(index)->text());
		param_list = msg->getParameterList();
	}
	else
	{
		CanSignal *sig = m_dbc->getSignal(m_model->itemFromIndex(index)->text());
		param_list = sig->getParameterList();
	}


	for (int i = 0; i < m_details->rowCount(); i++)
	{

		if(i < param_list.length())
		{
			m_details->setRowHidden(i, false);

			// First column
			QTableWidgetItem *item;

			item = m_details->item(i,0);
			item->setText(param_list.at(i).first); // Parameter name

			item = m_details->item(i,1);
			item->setText(param_list.at(i).second); // Parameter value

		}
		else
		{
			m_details->setRowHidden(i, true);
		}
	}
}

