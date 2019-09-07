#include "pannel_canmsgdetails.h"



CanMsgWidget::CanMsgWidget(DbcModel* dbc, QWidget* parent) : QWidget(parent)
{
	m_dbc = dbc;
	m_idItem = new QStandardItem("ID");
	m_signalsItem = new QStandardItem("Signals");
	m_comment = new QStandardItem("Comment");

	m_model = new QStandardItemModel(this);
	m_model->setHorizontalHeaderItem(0, new QStandardItem("Message details"));
	m_model->appendRow(m_idItem);
	m_model->appendRow(m_signalsItem);

	m_tree = new QTreeView(this);
	m_tree->setModel(m_model);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(m_tree);

	setLayout(layout);
}

void CanMsgWidget::setDbcModel(DbcModel* dbc)
{
	m_dbc = dbc;
}

void CanMsgWidget::setMessage(const CanTraceMessage &msg)
{
	// Clear the current informations
	m_idItem->setText("ID");
	m_idItem->removeRows(0,m_idItem->rowCount()); // Remove the items
	m_signalsItem->removeRows(0, m_signalsItem->rowCount());

	if(m_dbc == 0 || isHidden())
		return; // No dbc related to that trace or widget not visible

	CanMessage* msg_def = m_dbc->getMessage(msg.id);

	if (msg_def == 0) // Message not found in the DBC
		return;

	// Update the informations about that message
	m_idItem->setText("ID : 0x" + QString::number(msg_def->getId(),16).toUpper() + " (" + msg_def->getName() + ")");

	//QList<QPair<QString, QString> > sig_list = msg_def->formatSignals(msg.data);
	QList<SignalValue_t> sig_list = msg_def->getSignalsValue(msg.data);

	for (int i = 0; i < sig_list.length(); i++)
	{
		m_signalsItem->appendRow(new QStandardItem(sig_list.at(i).name + " : " + sig_list.at(i).valueStr + " " + sig_list.at(i).unit));
	}

	// Comment
	m_comment->removeRows(0, m_comment->rowCount());
	QString comment = m_dbc->getMessage(msg.id)->getComment();

	if(!comment.isEmpty())
		m_comment->appendRow(new QStandardItem(comment));

	m_tree->expandAll();
}
