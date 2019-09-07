#include "model_cantrace.h"
#include "string_manip.h"

CanTraceModel::CanTraceModel(QObject *parent) : QAbstractTableModel(parent)
{
	m_dbc = 0;
	currentSelectedMsgId = -1;
	m_nb_msg = 0;

	m_offsetMsgNumber = true;
	m_offsetTimestamp = true;

	// Timer for refreshing views
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(refreshViews()));
	m_timer->setInterval(400); // in ms
}

QStringList CanTraceModel::getErrors()
{
	QStringList errors = m_errors;
	m_errors.clear(); // Clear the error list

	return errors;
}

int CanTraceModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return m_messages.length();
}

int CanTraceModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return TRACE_NB_COLUMNS;
}

QVariant CanTraceModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		   return QVariant();

	CanTraceMessage msg = m_messages.at(index.row());
	CanTraceMessage first_msg = m_messages.at(0); // Index should be valide, function should not be called if m_message.count is 0

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case TRACE_COL_NUMBER:
				//return QString::number(index.row() + 1); // Used in CanTraceWidget::SelectionChangedSlot (TODO : find another way to reference the packets)
				if(m_offsetMsgNumber)
					return QString::number(msg.number - first_msg.number + 1);
				else
					return QString::number(msg.number);

			case TRACE_COL_TIMESTAMP: // Timestamp
				if(m_offsetTimestamp)
					return QString::number(msg.timestamp - first_msg.timestamp,'f',4);
				else
					return QString::number(msg.timestamp, 'f', 4);

			case TRACE_COL_TYPE: // Type : Rx, Tx
				return msg.type;
				break;

			case TRACE_COL_ID: // Message ID : {message name}
				if (hasDbc())
				{
					// return id + message name
					CanMessage *msg_p = m_dbc->getMessage(msg.id);

					if (msg_p != 0)
						return "0x" + QString().sprintf("%03x",msg.id).toUpper() + " : " + m_dbc->getMessage(msg.id)->getName();
				}

				return "0x" + QString().sprintf("%03x",msg.id).toUpper();
				break;

			case TRACE_COL_DATA_LEN: // Data Length
				return QString::number(msg.data.size());
				break;

			case TRACE_COL_DATA: // Data bytes
				QString data_str;
				for (int i = 0; i < msg.data.size(); i++)
				{
					data_str.append("  ").append(QString().sprintf("%02x",msg.data.at(i)).toUpper());
				}
				return data_str;
				break;
		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == TRACE_COL_DATA)
			return Qt::AlignLeft;
		else if (index.column() == TRACE_COL_ID && hasDbc())
			return Qt::AlignLeft;
		else
			return Qt::AlignCenter;
	}
	else if (role == Qt::ForegroundRole)
	{
		if(hasDbc())
		{
			if(TRACE_COL_DATA_LEN == index.column())
			{
				CanMessage *msg_p = m_dbc->getMessage(msg.id);
				if(msg_p != 0)
				{
					if(msg.data.length() != msg_p->getDataLenght())
					{
						return QBrush(Qt::red);
					}
				}
			}
		}
	}
	else if (role == Qt::BackgroundRole)
	{
		// Highlight message with same ID as selected message
		if(msg.id == currentSelectedMsgId)
			return QBrush(QColor(0,85,123,51));
	}
	else if(role == Qt::UserRole)
	{
		return index.row(); // Message index
	}

	return QVariant();
}


QVariant CanTraceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(Qt::Horizontal == orientation)
		{
			// Horizontal headers

			switch (section)
			{
				case TRACE_COL_NUMBER:
					return "#";
					break;

				case TRACE_COL_TIMESTAMP: // Timestamp
					return "Time stamp (s)";
					break;

				case TRACE_COL_TYPE: // Type : Rx, Tx
					return "Type";
					break;

				case TRACE_COL_ID: // Message ID : {message name}
					return "Msg ID";
					break;

				case TRACE_COL_DATA_LEN: // Data Length
					return "DLC";
					break;

				case TRACE_COL_DATA: // Data bytes
					return "Data";
					break;
			}

		}
		else if (role == Qt::ToolTipRole && section == TRACE_COL_ID)
		{
			return "Data Length (bytes)";
		}
		else if (orientation == Qt::Vertical)
		{
			return QVariant(); // No vertical header
			//return QAbstractTableModel::headerData(section, orientation, role); // Original implementation (row numbers)
		}
	}
	else
	{
		return QAbstractTableModel::headerData(section, orientation, role); // Original implementation
	}

	return QVariant();
}


void CanTraceModel::addMessage(const CanTraceMessage & msg)
{
	// Add the message to the message list.
	// Do not notify for model change here as this function might be called too fast. It will overload the view drawing.

	m_messages.append(msg);


	// Start the timer which timeout will call for view update

	if(!m_timer->isActive())
		m_timer->start();

	//QDateTime::currentDateTime().toTime_t(); // Attribute timestamp if not (timestamp in interface)

	//beginInsertRows(QModelIndex(), m_messages.count(), m_messages.count());
	//endInsertRows();

}

bool CanTraceModel::loadTraceFile_PCAN(QString file_name)
{
	m_trace_file = file_name;

	QFile file(file_name);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_errors.append("Unable to open file  " + file_name);
		return false;
	}

	emit beginResetModel();

	m_messages.clear();

	QTextStream stream(&file);
	int file_version = 1; // 1 = 1.1

	while (!stream.atEnd())
	{
		QString line = stream.readLine().simplified();

		if(line.startsWith(';'))
		{
			// Comment line
			if(line.contains("$FILEVERSION="))
			{
				if(line.contains("2.0"))
				{
					file_version = 2;
				}
			}
		}
		else
		{
			CanTraceMessage msg = parseTraceLine_PCAN(line, file_version);
			//addMessage(msg); // This updates the view for each added message
			m_messages.append(msg); // Add message
		}
	}

	file.close();

	emit endResetModel();

	return true;
}

bool CanTraceModel::loadTraceFile_BusMaster(QString file_name)
{
	m_trace_file = file_name;

	QFile file(file_name);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_errors.append("Unable to open file  " + file_name);
		return false;
	}

	emit beginResetModel();

	m_messages.clear();

	QTextStream stream(&file);

	double start_timestamp = -1;
	while (!stream.atEnd())
	{
		QString line = stream.readLine().simplified();

		if(line == "")
			continue; // Ignore empty lines

		if(line.startsWith("***"))
		{
			// Comment line
		}
		else
		{
			CanTraceMessage msg = parseTraceLine_BusMaster(line);

			if(m_messages.count() == 0)
			{
				start_timestamp = msg.timestamp;
				msg.timestamp = 0;
			}
			else
				msg.timestamp -= start_timestamp; // Correct timestamp (to start with 0)

			//addMessage(msg);
			m_messages.append(msg);
		}
	}

	file.close();

	emit endResetModel();

	return true;
}

bool CanTraceModel::loadTraceFile_WiresharkExport(QString file_name)
{
	m_trace_file = file_name;

	QFile file(file_name);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_errors.append("Unable to open file  " + file_name);
		return false;
	}

	emit beginResetModel();

	m_messages.clear();

	QTextStream stream(&file);

	double start_timestamp = -1;
	while (!stream.atEnd())
	{
		QString line = stream.readLine().simplified();

		if(line == "")
			continue; // Ignore empty lines

		if(line.contains("No"))
		{
			// Header line
		}
		else
		{
			CanTraceMessage msg = parseTraceLine_WiresharkExport(line);

			if(m_messages.count() == 0)
			{
				start_timestamp = msg.timestamp;
				msg.timestamp = 0;
			}
			else
				msg.timestamp -= start_timestamp; // Correct timestamp (to start with 0)

			//addMessage(msg);
			m_messages.append(msg);
		}
	}

	file.close();

	emit endResetModel();

	return true;
}

CanTraceMessage CanTraceModel::parseTraceLine_WiresharkExport(const QString &line_)
{
	// 000102030405060708090a0b0c0d0e0f10

	QString line = line_.simplified();
	CanTraceMessage msg;

	bool ok;

	// Message number
	msg.number = line.left(8).toUInt(&ok, 16);
	line.remove(0, 8);

	// Timestamp
	msg.timestamp = line.left(8).toUInt(&ok, 16) / 10000;
	line.remove(0, 8);

	msg.type = "Rx"; // Message type

	msg.id = line.left(8).toUInt(&ok, 16); // Message ID
	line.remove(0,8);

	int data_len = line.left(2).toUInt(&ok, 16);

	if(data_len > 8)
		data_len = 8;

	msg.data.clear();
	for (int i = 0; i < data_len; i++)
	{
		int val = line.left(2).toShort(0, 16);
		line.remove(0,2);
		msg.data.append(val);
	}

	return msg;
}

CanTraceMessage CanTraceModel::parseTraceLine_PCAN(const QString &line_, const int file_version)
{
	QString line = line_;
	CanTraceMessage msg;

	if(file_version == 2)// Version 1.1 of Peak trace file
	{
		//line = "     1        36.200 DT     0103 Tx 4  01 00 00 00"

		line = line.simplified(); // Remove extra white space

		//line = "1 36.200 DT 0103 Tx 4 01 00 00 00"

		msg.number = singleSplit(' ', line).toInt();			// Message number
		msg.timestamp = singleSplit(' ', line).toDouble()/1000;	// Time offset
		QString typeID = singleSplit(' ', line);				// Type ID

		if(typeID == "DT")
			msg.id = singleSplit(' ', line).toUInt(0, 16);			// Message ID

		msg.type = singleSplit(' ', line);						// Message type


		int data_len;

		if(typeID == "DT")
			data_len = singleSplit(' ',line).toInt();
		else
			data_len = 0;

		msg.data.clear();
		for (int i = 0; i < data_len; i++)
		{
			int val = singleSplit(' ', line).toShort(0, 16);
			msg.data.append(val);
		}
	}
	else
	{
		// 2)       612.0  Rx         0481  8  0E 9D 0E 9D 0E 9D 0E A0

		msg.number = singleSplit(") ", line).toInt(); // Message number
		msg.timestamp = singleSplit(' ', line).toDouble()/1000; // Time offset
		msg.type = singleSplit(' ', line);
		msg.id = singleSplit(' ', line).toUInt(0, 16);

		int data_len = singleSplit(' ',line).toInt();

		msg.data.clear();
		for (int i = 0; i < data_len; i++)
		{
			int val = singleSplit(' ', line).toShort(0, 16);
			msg.data.append(val);
		}
	}

	return msg;
}

CanTraceMessage CanTraceModel::parseTraceLine_BusMaster(const QString &line_)
{
	// 08:50:22:5012 Rx 1 0x048 s 8 0F 6E 0F 53 01 0F 00 03

	QString line = line_;
	CanTraceMessage msg;

	// Timestamp
	QString timestamp_str = singleSplit(' ', line); // Extract timestamp from line
	QStringList list = timestamp_str.split(':');

	if(list.count() == 4)
	{
		QTime time = QTime(list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt() / 10);
		msg.timestamp = time.msecsSinceStartOfDay() / 1000.0;
	}
	else if (list.count() == 3)
		msg.timestamp = QTime(list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt()).msecsSinceStartOfDay() / 1000.0;

	msg.type = singleSplit(' ', line); // Type (Rx / Tx)
	singleSplit('x',line); // Hex indicator
	msg.id = singleSplit(' ', line).toUInt(0, 16); // Message ID

	singleSplit(' ',line); // other Type we do not care about for now

	int data_len = singleSplit(' ',line).toInt(); // Data Length

	msg.data.clear();
	for (int i = 0; i < data_len; i++)
	{
		int val = singleSplit(' ', line).toShort(0, 16);
		msg.data.append(val);
	}
	return msg;
}

void CanTraceModel::setDbcModel(DbcModel *dbc)
{
	if (m_dbc != dbc)
	{
		emit beginResetModel();
		m_dbc = dbc;
		emit endResetModel();
	}
}

void CanTraceModel::clearDbcModel()
{
	setDbcModel(0);
}

bool CanTraceModel::hasDbc() const
{
	if (m_dbc != 0)
		return true;
	else
		return false;
}

int CanTraceModel::count()
{
	return m_messages.count();
}

CanTraceMessage CanTraceModel::getMessage(int row)
{
	return m_messages.at(row);
}

QString CanTraceModel::getFilePath()
{
	return m_trace_file;
}

void CanTraceModel::setCurrentSelectedMsgId(quint32 msg_id)
{
	emit layoutAboutToBeChanged();
	currentSelectedMsgId = msg_id;
	emit layoutChanged();
}

void CanTraceModel::refreshViews()
{
	if(m_nb_msg - m_messages.count() == 0) // No new messages were received since last function call
	{
		m_timer->stop();
		return;
	}

	// Indicate that the model has new rows (view will update)
	beginInsertRows(QModelIndex(),m_nb_msg, m_messages.count() - 1);
	endInsertRows();

	m_nb_msg = m_messages.count();
}

void CanTraceModel::useMessageNumberOffset(bool enable)
{
	m_offsetMsgNumber = enable;
	dataChanged(index(0,TRACE_COL_NUMBER), index(rowCount(), TRACE_COL_NUMBER));
}

void CanTraceModel::useTimestampOffset(bool enable)
{
	m_offsetTimestamp = enable;
	dataChanged(index(0,TRACE_COL_TIMESTAMP), index(rowCount(), TRACE_COL_TIMESTAMP));
}


const QList<CanTraceMessage>& CanTraceModel::getAllMessages()
{
	return m_messages;
}
