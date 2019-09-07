#include <iterator>
#include "string_manip.h"
#include "struct_dbc.h"
#include <QDebug>

DbcModel::DbcModel(QString file_path)
{
	m_file_path = file_path;
	loadDbcFile();
}

DbcModel::~DbcModel()
{
	for (QHash<quint32, CanMessage*>::iterator i = m_messages.begin(); i != m_messages.end(); i++)
	{
		delete *i;
	}

	for (QHash<QString, CanSignal*>::iterator j = m_signals.begin(); j != m_signals.end(); j++)
	{
		delete *j;
	}
}

void DbcModel::loadDbcFile()
{
	QFile dbc_file(m_file_path);

	if(!dbc_file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream data(&dbc_file);

	QString line;

	while (!data.atEnd())
	{
		line = data.readLine().simplified(); // Read line and remove leading and trailing white spaces/tabs/newline

		if (line.isEmpty()) // Filter empty lines
			continue;

		while(line.startsWith("BO_ ")) // Message definition
		{
			CanMessage* msg = create_message(line); // Create message from that line
			m_messages[msg->getId()] = msg; // Add message to message list

			// The next lines are signals definitions
			while (!data.atEnd())
			{
				line = data.readLine().simplified(); // Read line and remove leading and trailing white spaces/tabs/newline

				if (line.isEmpty()) // Filter empty lines
					continue;

				if (line.startsWith("SG_ ")) // Signal definition
				{
					CanSignal* sig = parse_signal(line);

					// Add signal to signal list if not already in the list
					// (The same signal can appear in different messages)

					if (sig != NULL)
						if(!m_signals.contains(sig->getName()))
							m_signals[sig->getName()] = sig;

						msg->appendSignal(sig);// Add signal to message
				}
				else
				{
				   break;
				}
			}
		}

		if (line.startsWith("CM_ ")) // Comment
		{


			while (!line.contains(";"))
				line += data.readLine(); // Comment ends by ';' and may contain '\n'

			parse_comment(line);
		}
		else if (line.startsWith("SIG_VALTYPE_ "))
		{
			parse_valtype(line);
		}
		else if (line.startsWith("VAL_ ")) // Value table
		{
			parse_valtable(line);
		}
		else
		{
			// Unsupported line
		}
	}

	dbc_file.close();

	// Set signals message links (link from signals to related messages)
	QHash<quint32, CanMessage*>::iterator it;
	for(it = m_messages.begin(); it != m_messages.end(); it++)
	{
		CanMessage* msg = it.value();
		QList< QPair<CanSignal*, quint8> > sig_list = msg->getSignals();

		if(msg->getName() == "VECTOR__INDEPENDENT_SIG_MSG")
			continue; // Ignore orphan signals

		for(int i = 0; i < sig_list.count(); i ++)
		{
			sig_list.at(i).first->addRelatedMessage(msg->getId());
		}
	}
}


CanMessage* DbcModel::create_message(QString line_)
{
	QString line = line_; // Keep the original line value

	// Format : BO_ message_id Message_Name : data_length Node1,Node1
	QStringList list = line.split(':');
	QStringList header = list.at(0).simplified().split(' ');
	QStringList footer = list.at(1).simplified().split(' '); // Second part of the line

	// header.at(0) = BO_
	// header.at(1) = message ID
	// header.at(2) = Message name
	// footer.at(0) = Message length
	// footer.at(1) = Node 0
	// footer.at(2) = Node 1

	CanMessage* msg = new CanMessage(header.at(2), header.at(1).toUInt(), footer.at(0).toUShort());

	if (m_messages.contains(msg->getId()))
	{
		// Double message ID (should not happen)
		m_errors.append("Error parsing line :" + line_ + " (Message ID already exists)");
		return NULL;
	}

	// Transmitter nodes... (todo)

	return msg;
}



CanSignal* DbcModel::parse_signal(QString line_)
{
	//  SG_ New_Signal_1 : 8|8@1- (1,7) [0|0] "mA"  Node_2,Node_1
	// SG_ Signal_name : {start bit}|{no_of_bits}@{bit_order}{type} (factor,offset) [min|max] "units" {receiver nodes}

	QString line = line_; // Keep the original line_ value

	singleSplit(' ',line);

	QString name = singleSplit(' ',line).simplified();
	line = line.remove(0,1).simplified(); // Remove the ':'
	quint8 start_bit = singleSplit('|', line).simplified().toUShort();
	quint8 len_bit = singleSplit('@', line).simplified().toUShort();

	Endianness_t byte_order;
	if(extract(1,line) == "1")
		byte_order = INTEL;
	else
		byte_order = MOTO;

	Signess_t sign;
	if(extract(1,line) == "-")
		sign = SIGNED;
	else
		sign = UNSIGNED;

	singleSplit('(', line);
	double factor = singleSplit(',', line).simplified().toDouble();
	double offset = singleSplit(')', line).simplified().toDouble();
	singleSplit('[', line);
	double min = singleSplit('|', line).simplified().toDouble();
	double max = singleSplit(']',line).simplified().toDouble();

	singleSplit('"', line);
	QString units = singleSplit('"', line);

	// Receiver Nodes... (todo)

	CanSignal* sig = new CanSignal(name, min, max, len_bit, units, byte_order, sign, start_bit, offset, factor);

	return sig;
}


void DbcModel::parse_comment(QString line_)
{
	//Ex : CM_ SG_ 165 NLG5_OC_BO "Output current of booster
	//	   Resolution : 0.01 A";

	QString line = line_; // Conserve original line

	singleSplit(' ',line); // Remove CM_

	QString type = singleSplit(' ', line);

	if(type == "SG_") // Comment related to a signal
	{
		singleSplit(' ', line); // Remove the message number
		QString name = singleSplit(' ', line).simplified();
		QStringList list = line.split('"'); // Extract the comment

		if (list.length() < 3)
		{
			m_errors.append("Error parsing comment : " + line_ + " (will be ignored)");
			return;
		}

		if (m_signals.contains(name))
		{
			m_signals[name]->setComment(list.at(1).trimmed());
		}
		else
		{
			m_errors.append("Error : signal \"" + name + "\" not found. Comment related to that signal will be ignored");
		}
	}
	else if (type == "BO_") // Comment related to a message
	{
		quint32 msg_id = singleSplit(' ', line).toUShort();
		QStringList list = line.split('"'); // Extract the comment

		if (list.length() < 3)
		{
			m_errors.append("Error parsing comment : " + line_ + " (will be ignored)");
			return;
		}

		if (m_messages.contains(msg_id))
		{
			m_messages[msg_id]->setComment(list.at(1).trimmed());
		}
		else if(msg_id != 0)
		{
			m_errors.append("Error : message id \"" + QString::number(msg_id) + "\" not found. Comment related to that signal will be ignored");
		}
	}
	else
	{
		// Not supported
	}

}



CanMessage* DbcModel::getMessage(quint32 id)
{
	if (m_messages.contains(id))
		return m_messages[id];
	else
		return NULL;
}


// Avoid using this function as much as possible (slow), use getMessage(quint16 id) if possible
CanMessage* DbcModel::getMessage(const QString& name)
{
	for (QHash<quint32, CanMessage*>::iterator it = m_messages.begin(); it != m_messages.end(); it++)
	{
		if(it.value()->getName() == name)
		{
			return (*it);
		}
	}

	return NULL;
}

QList<quint32> DbcModel::getMessagesId()
{
	return m_messages.keys();
}



CanSignal* DbcModel::getSignal(const QString& name)
{
	if(m_signals.contains(name))
		return m_signals[name];
	else
		return NULL;
}

QStringList DbcModel::getSignalList()
{
	return m_signals.keys();
}

QStringList DbcModel::getMessageNames()
{
	QList<CanMessage*> msglist = m_messages.values(); // All messages objects
	QStringList nameList; // All messages names

	for(int i = 0; i < msglist.count(); i++)
	{
		nameList << msglist.at(i)->getName();
	}

	return nameList;
}


QStringList DbcModel::getErrors()
{
	QStringList errors = m_errors;
	m_errors.clear();

	return errors;
}


QString DbcModel::getFileName()
{
	return m_file_path.split('/').last().split('\\').last();
}

QString DbcModel::getFullPath()
{
	return m_file_path;
}


void DbcModel::parse_valtype(QString line_)
{
	// SIG_VALTYPE_ 6 Sig_double : 2;
	// SIG_VALTYPE_ 5 Sig_float : 1;


	if(line_.split(' ').length() < 4)
		return;

	QString line = line_;

	singleSplit(' ', line); // Remove the SIG_VALTYPE_

	quint8 msg_id = singleSplit(' ', line).toUInt();
	QString sig_name = singleSplit(' ', line);
	QString type_code = line.remove(':').remove(';').simplified();

	if(!m_messages.contains(msg_id))
	{
		m_errors.append("Unknown message ID :" + QString::number(msg_id) + "for SIG_VALTYPE_ definition");
		return;
	}

	CanMessage *msg = m_messages[msg_id];
	CanSignal *sig = msg->getSignal(sig_name);

	if(sig == 0)
	{
		m_errors.append("Error : The signal value type is defined for a signal name ("
						+ sig_name
						+ ") that is not part of the specified message id("
						+ QString::number(msg_id)
						+ ")"
						);
		return;
	}


	if (type_code == "1")
		sig->setSigness(FLOAT_);
	else if (type_code == "2")
		sig->setSigness(DOUBLE_);

}

void DbcModel::parse_valtable(QString line_)
{
	// Line example :
	// VAL_ 73 sig_Interlock_State 1 "Interlock opened" 0 "Interlock closed" ;

	QString line = line_;

	line = singleSplit(';',line).simplified(); // Keep what is before the ';'
	singleSplit(' ', line); // remove the VAL_

	//qDebug() << line;

	// Get the message ID
	bool ok;
	quint32 message_id = singleSplit(' ', line).toULongLong(&ok); // Message ID


	//qDebug() << line;

	if(!ok)
	{
		m_errors.append("Can't convert the message ID to an interger for line: " + line_);
		return;
	}

	// Get the signal name
	QString sig_name = singleSplit(' ', line);

	//qDebug() << line;

	QStringList valtable_str = line.simplified().split('"');
	valtable_str.pop_back(); // Remove the empty string

	//qDebug() << valtable_str;

	QHash<quint64, QString> valtable;

	if(valtable_str.count() % 2 != 0)
	{
		m_errors.append("Can't parse value tables, uneven number of arguments for line: " + line_);
		return;
	}

	for(int i = 0; i < valtable_str.length(); i+=2)
	{

		//qDebug() << valtable_str.length() << i;
		quint64 value = valtable_str.at(i).toULongLong();
		QString description = valtable_str.at(i+1);
		description = description.remove('"').simplified();

		//qDebug() << value << description;
		valtable[value] = description;
	}

	CanMessage *msg = getMessage(message_id);

	if(msg == 0) // Message not found for that ID
		return;

	CanSignal *sig = msg->getSignal(sig_name);

	if(sig == 0)
		return;

	sig->setValueTable(valtable);

	//qDebug() << valtable;
	//qDebug() << "\n";
}


bool DbcModel::containsSignal(const QString& signal_name)
{
	return getSignalList().contains(signal_name);
}


