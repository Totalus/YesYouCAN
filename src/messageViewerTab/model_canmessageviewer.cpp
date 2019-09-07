#include "model_canmessageviewer.h"

CanMessageViewerModel::CanMessageViewerModel(QObject *parent) : QAbstractTableModel(parent)
{
	m_dbc = 0;
	m_nb_msg = 0;

	// Timer for refreshing views
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(refreshViews()));
	m_timer->setInterval(100); // in ms

}


void CanMessageViewerModel::newMessage(CanTraceMessage msg)
{
	CanFixedTraceMessage new_msg;

	if(m_messages.contains(msg.id))
	{
		new_msg = m_messages[msg.id];
	}
	else
	{
		// Init new CAN Fixed Trace Message information
		new_msg.count = 0;
		new_msg.period_ms = 0;
		new_msg.timestamp_previous = 0;
		new_msg.disp_signals = false;
	}

	new_msg.timestamp_previous = new_msg.last_msg.timestamp;// Update older timestamps
	new_msg.last_msg = msg;									// Update message information
	new_msg.count++;										// Increment count

	// Calculate period
	new_msg.period_ms = (msg.timestamp * 1000 - new_msg.timestamp_previous * 1000);

	m_messages[msg.id] = new_msg;

	if(!m_timer->isActive())
		m_timer->start();

	//beginResetModel();
	//endResetModel();
	//emit dataChanged(index(m_messages.keys().indexOf(msg.id),0),index(m_messages.keys().indexOf(msg.id), FTRACE_COL_SIGNALS));
}



void CanMessageViewerModel::setDbcModel(DbcModel *dbc)
{
	if (m_dbc != dbc)
	{
		emit beginResetModel();
		m_dbc = dbc;
		emit endResetModel();
	}
}

void CanMessageViewerModel::clearDbcModel()
{
	setDbcModel(0);
}

bool CanMessageViewerModel::hasDbc() const
{
	if (m_dbc != 0)
		return true;
	else
		return false;
}

int CanMessageViewerModel::count()
{
	return m_messages.count();
}

QStringList CanMessageViewerModel::getErrors()
{
	QStringList errors = m_errors;
	m_errors.clear(); // Clear the error list

	return errors;
}

int CanMessageViewerModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	return m_messages.count();
}

int CanMessageViewerModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	if (m_dbc == 0)
		return FTRACE_NB_COLUMNS - 1; // Do not display signal column

	return FTRACE_NB_COLUMNS;
}

QVariant CanMessageViewerModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		   return QVariant();

	CanFixedTraceMessage msg = m_messages.values().at(index.row());

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case FTRACE_COL_TYPE: // Type : Rx, Tx
				return msg.last_msg.type;
				break;

			case FTRACE_COL_ID: // Message ID : {message name}
				if (hasDbc())
				{
					// return id + message name
					CanMessage *msg_p = m_dbc->getMessage(msg.last_msg.id);

					if (msg_p != 0)
						return "0x" + QString::number(msg.last_msg.id,16).toUpper() + " : " + m_dbc->getMessage(msg.last_msg.id)->getName();
				}

				return "0x" + QString::number(msg.last_msg.id,16).toUpper();
				break;

			case FTRACE_COL_DATA_LEN: // Data Length
				return QString::number(msg.last_msg.data.size());
				break;

			case FTRACE_COL_DATA: // Data bytes
				// Use brackets to limit the variables scope
				{
					QString data_str;
					for (int i = 0; i < msg.last_msg.data.size(); i++)
					{
						if(i == 0)
							data_str.append(QString().sprintf("%02x",msg.last_msg.data.at(i)).toUpper());
						else
							data_str.append(" ").append(QString().sprintf("%02x",msg.last_msg.data.at(i)).toUpper());
					}
					return data_str;
				}
				break;

			case FTRACE_COL_COUNT: // Count for this message
				return QString::number(msg.count);
				break;

			case FTRACE_COL_PERIOD:
				return QString::number(msg.period_ms);
				break;

			case FTRACE_COL_SIGNALS:
				if(msg.disp_signals)
				{
					QString s;
					CanMessage *msg_def = m_dbc->getMessage(msg.last_msg.id);

					if(msg_def == 0)
						return "Unkown ID";

					QList<SignalValue_t> sig_list = msg_def->getSignalsValue(msg.last_msg.data);

					//QList< QPair<QString, QString> > sig_list = msg_def->formatSignals(msg.last_msg.data);

					for(int i = 0; i < sig_list.count(); i++)
					{
						if(i > 0)
							s += "\n";

						s += sig_list.at(i).name + " : " + sig_list.at(i).valueStr + " " + sig_list.at(i).unit;
					}

					return s;
				}

				break;

		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == FTRACE_COL_DATA || index.column() == FTRACE_COL_SIGNALS)
			return Qt::AlignLeft;
		else if (index.column() == FTRACE_COL_ID && hasDbc())
			return Qt::AlignLeft;
		else
			return Qt::AlignHCenter;
	}
	else if (role == Qt::ForegroundRole)
	{
		if(hasDbc())
		{
			if(FTRACE_COL_DATA_LEN == index.column())
			{
				CanMessage *msg_p = m_dbc->getMessage(msg.last_msg.id);
				if(msg_p != 0)
				{
					if(msg.last_msg.data.length() != msg_p->getDataLenght())
					{
						return QBrush(Qt::red);
					}
				}
			}
		}
	}
	else if (role == Qt::CheckStateRole)
	{
		if(index.column() == FTRACE_COL_SIGNALS)
			return (msg.disp_signals ? Qt::Checked : Qt::Unchecked);
	}


	return QVariant();
}


QVariant CanMessageViewerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(Qt::Horizontal == orientation)
		{
			// Horizontal headers

			switch (section)
			{
				case FTRACE_COL_TYPE: // Type : Rx, Tx
					return "Type";
					break;

				case FTRACE_COL_ID: // Message ID : {message name}
					return "Msg ID";
					break;

				case FTRACE_COL_DATA_LEN: // Data Length
					return "DLC";
					break;

				case FTRACE_COL_DATA: // Data bytes
					return "Data";
					break;

				case FTRACE_COL_PERIOD:
					return "Period (ms)";
					break;

				case FTRACE_COL_SIGNALS:
					return "Signals";
					break;

				case FTRACE_COL_COUNT:
					return "Count";
					break;
			}

		}
		else if (orientation == Qt::Vertical)
		{
			//return QAbstractItemModel::headerData(section, orientation, role); // Original implementation (row numbers)
			//return QVariant(); // No header
		}
	}
	else
	{
		return QAbstractItemModel::headerData(section, orientation, role); // Original implementation
	}

	return QVariant();
}

Qt::ItemFlags CanMessageViewerModel::flags(const QModelIndex &index) const
{
	return (QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable);
}

bool CanMessageViewerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid())
		return QAbstractTableModel::setData(index, value, role);

	if(index.column() == FTRACE_COL_SIGNALS)
	{
		m_messages[m_messages.keys().at(index.row())].disp_signals = (value.toInt() == Qt::Checked ? true : false);
		emit dataChanged(index, index);
	}

	return QAbstractTableModel::setData(index, value, role);
}

void CanMessageViewerModel::refreshViews()
{
	if(m_nb_msg < m_messages.count())
	{
		// Indicate that the model has new rows (view will update)
		beginInsertRows(QModelIndex(),m_nb_msg, m_messages.count() - 1);
		endInsertRows();
	}

	emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));

	m_nb_msg = m_messages.count();
}

void CanMessageViewerModel::resetData()
{
	beginResetModel();
	m_messages.clear();
	endResetModel();
}
