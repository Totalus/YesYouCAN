#include "model_cantransmit.h"

CanTransmitterModel::CanTransmitterModel(QObject *parent) : QAbstractTableModel(parent)
{
	m_dbc = 0;
	m_device = 0;

	// Timer for refreshing views
	//m_timer = new QTimer(this);
	//connect(m_timer, SIGNAL(timeout()), this, SLOT(refreshViews()));
	//m_timer->setInterval(100); // in ms
}


void CanTransmitterModel::setDbcModel(DbcModel *dbc)
{
	if (m_dbc != dbc)
	{
		emit beginResetModel();
		m_dbc = dbc;
		emit endResetModel();
	}
}

void CanTransmitterModel::clearDbcModel()
{
	setDbcModel(0);
}

bool CanTransmitterModel::hasDbc() const
{
	if (m_dbc != 0)
		return true;
	else
		return false;
}

int CanTransmitterModel::count()
{
	return m_messagesObjects.count();
}

QStringList CanTransmitterModel::getErrors()
{
	QStringList errors = m_errors;
	m_errors.clear(); // Clear the error list

	return errors;
}

int CanTransmitterModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	return m_messagesObjects.count();
}

int CanTransmitterModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return TX_NB_COLUMNS;
}

QVariant CanTransmitterModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		   return QVariant();

	if(index.row() >= m_messagesObjects.count())
	{
		qWarning() << "Index row is higher or equal than number of rows";
		return QVariant();
	}

	CanTxMessageObject *msgObject = m_messagesObjects.at(index.row());
	CanTraceMessage trMsg = m_messagesObjects.at(index.row())->getCanMessage();

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case TX_COL_ID: // Message ID : {message name}
				if (hasDbc())
				{
					// return id + message name
					CanMessage *msg_p = m_dbc->getMessage(trMsg.id);

					if (msg_p != 0)
						return "0x" + QString::number(trMsg.id,16).toUpper() + " : " + m_dbc->getMessage(trMsg.id)->getName();
				}

				return "0x" + QString::number(trMsg.id,16).toUpper();
				break;

			case TX_COL_DATA_LEN: // Data Length
				return QString::number(trMsg.data.size());
				break;

			case TX_COL_DATA: // Data bytes
				// Use brackets to limit the variables scope
				{
					QString data_str;
					for (int i = 0; i < trMsg.data.size(); i++)
					{
						if(i == 0)
							data_str.append(QString().sprintf("%02x",trMsg.data.at(i)).toUpper());
						else
							data_str.append(" ").append(QString().sprintf("%02x",trMsg.data.at(i)).toUpper());
					}
					return data_str;
				}
				break;

			case TX_COL_COUNT: // Count for this message
				return QString::number(msgObject->getCount());
				break;

			case TX_COL_PERIOD:
				return QString::number(msgObject->getPeriod());
				break;

			case TX_COL_SEND:
				if(msgObject->getPeriod() == 0)
					return "Manual";
				else if(msgObject->getTransmit())
					return "Sending";
				else
					return QVariant();
				break;

			case TX_COL_COMMENT:
				return msgObject->getComment();
				break;

		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == TX_COL_DATA || index.column() == TX_COL_SEND)
			return Qt::AlignLeft + Qt::AlignVCenter;
		else if (index.column() == TX_COL_ID && hasDbc())
			return Qt::AlignLeft + Qt::AlignVCenter;
		else
			return Qt::AlignCenter;
	}
	else if (role == Qt::ForegroundRole)
	{
		if(hasDbc())
		{
			if(TX_COL_DATA_LEN == index.column())
			{
				CanMessage *msg_p = m_dbc->getMessage(trMsg.id);
				if(msg_p != 0)
				{
					if(trMsg.data.length() != msg_p->getDataLenght())
					{
						return QBrush(Qt::red);
					}
				}
			}
		}
	}
	else if (role == Qt::CheckStateRole)
	{
		if(index.column() == TX_COL_SEND)
		{
			if(msgObject->getPeriod() == 0)
				return QVariant();
			else
				return (msgObject->getTransmit() ? Qt::Checked : Qt::Unchecked);
		}
	}

	return QVariant();
}


QVariant CanTransmitterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(Qt::Horizontal == orientation)
		{
			// Horizontal headers

			switch (section)
			{
				case TX_COL_ID: // Message ID : {message name}
					return "Msg ID";
					break;

				case TX_COL_DATA_LEN: // Data Length
					return "DLC";
					break;

				case TX_COL_DATA: // Data bytes
					return "Data";
					break;

				case TX_COL_PERIOD:
					return "Period (ms)";
					break;

				case TX_COL_SEND:
					return "Send";
					break;

				case TX_COL_COMMENT:
					return "Comment";
					break;

				case TX_COL_COUNT:
					return "Count";
					break;
			}

		}
		else if (orientation == Qt::Vertical)
		{
			// No header
		}
	}
	else
	{
		return QAbstractItemModel::headerData(section, orientation, role); // Original implementation
	}

	return QVariant();
}

Qt::ItemFlags CanTransmitterModel::flags(const QModelIndex &index) const
{
	if(index.column() == TX_COL_SEND && m_device == 0) // No hardware interface
	{
		return (QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled);
	}

	return (QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable);
}

bool CanTransmitterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid())
		return QAbstractTableModel::setData(index, value, role);

	if(index.column() == TX_COL_SEND)
	{
		m_messagesObjects.at(index.row())->setTransmit(value.toInt() == Qt::Checked ? true : false);
		emit dataChanged(index, index);
	}

	return QAbstractTableModel::setData(index, value, role);
}
/*
void CanTransmitModel::refreshViews()
{
	static int nb_msg = 0; // Number of messages in the model since last function call

	if(nb_msg < m_messagesObjects.count())
	{
		// Indicate that the model has new rows (view will update)
		beginInsertRows(QModelIndex(),nb_msg, m_messagesObjects.count() - 1);
		endInsertRows();
	}

	emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));

	nb_msg = m_messagesObjects.count();
}
*/

void CanTransmitterModel::updateMessageCount()
{
	// Use to update the view for count of messages
	CanTxMessageObject* senderObj = qobject_cast<CanTxMessageObject*>(QObject::sender());

	if(senderObj != 0)
	{
		int idx = m_messagesObjects.indexOf(senderObj);

		if(idx < 0)
			return;

		emit dataChanged(index(idx, TX_COL_COUNT), index(idx, TX_COL_COUNT)); // Update specific count cell in table
	}
}

void CanTransmitterModel::addCanMessage(CanTxMessageObject *msgObj)
{
	beginResetModel();

	msgObj->setParent(this);
	m_messagesObjects.append(msgObj);

	connect(msgObj, SIGNAL(sendMessage(CanTraceMessage&)), this, SLOT(updateMessageCount()));

	endResetModel();
}

CanTxMessageObject* CanTransmitterModel::getCanMessage(int row)
{
	if(row < m_messagesObjects.size())
		return m_messagesObjects.at(row);
	else
		return 0;
}

void CanTransmitterModel::removeMessage(int row)
{
	beginResetModel();

	if(row < m_messagesObjects.count())
	{
		delete m_messagesObjects.takeAt(row);
	}

	endResetModel();
}

void CanTransmitterModel::setDevice(HwInterface *iface)
{
	for(int i = 0; i < m_messagesObjects.count(); i++)
	{
		if(m_device != 0)
			m_messagesObjects.at(i)->disconnect(m_device, SLOT(writeMessage(CanTraceMessage&))); // Disconnect all signals of that object

		if(iface != 0)
			connect(m_messagesObjects.at(i), SIGNAL(sendMessage(CanTraceMessage&)), iface, SLOT(writeMessage(CanTraceMessage&)));
		else
			m_messagesObjects.at(i)->setTransmit(false); // Disable transmission when no interface
	}

	m_device = iface;
}
