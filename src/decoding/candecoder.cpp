#include "candecoder.h"

CanDecoder::CanDecoder(DbcModel *dbc, QObject *parent) : QObject(parent)
{
	m_dbc = dbc;
}

void CanDecoder::setDbc(DbcModel *dbc)
{
	m_msg_list.clear(); // Remove all registered messages
	m_dbc = dbc;

	if(m_dbc == 0)
		return;

	// For each signal already registered
	QStringList sigNames = m_sig_list.keys();

	for(int i = 0; i < sigNames.size(); i++)
	{
		// Add all related message to the registered message list

		CanSignal *sig = m_dbc->getSignal(sigNames.at(i));

		if(sig == NULL)
			continue;

		QList<quint32> msg = sig->getAssociatedMsgIDs();

		for(int i = 0; i < msg.size(); i++)
		{
			if(!m_msg_list.contains(msg.at(i)))
			{
				m_msg_list.append(msg.at(i));

				//qDebug() << "CAN Decoder : Registring msg" << msg.at(i);
			}
		}
	}
}

DbcModel* CanDecoder::getDbc()
{
	return m_dbc;
}

bool CanDecoder::registerSignal(QString sigName)
{
	if(m_dbc == 0)
		return false;

	// qDebug() << "CAN decoder : Registering signal" << sigName;

	// Add a signal to the registered signals and add the related
	// messages to a registered message list
	//

	if(sigName.isEmpty())
		return false;

	if(m_sig_list.contains(sigName)) // Already registered
	{
		m_sig_list[sigName]++; // Increase count
		return true;
	}

	// Add all related message to the registered message list

	CanSignal *sig = m_dbc->getSignal(sigName);

	if(sig == NULL)
		return false;

	QList<quint32> msg = sig->getAssociatedMsgIDs();

	for(int i = 0; i < msg.size(); i++)
	{
		if(!m_msg_list.contains(msg.at(i)))
			m_msg_list.append(msg.at(i));
	}

	m_sig_list[sigName] = 1; // Add signal to registered list

	//qDebug() << "Registered messages : " << m_msg_list;

	return true;
}

void CanDecoder::unregisterSignal(QString sigName)
{
	// Remove signal from signal list
	if(m_sig_list.contains(sigName))
	{
		m_sig_list[sigName]--;	// Decrement registration count
	}
	else
		return;

	if(m_sig_list[sigName] == 0)
	{
		if(m_dbc != 0)
		{
			// For all messages related to this signal
			QList<quint32> msgs = m_dbc->getSignal(sigName)->getAssociatedMsgIDs();
			for(int i = 0; i < msgs.size(); i++)
			{
				bool keepMessage = false;

				// For all other signals in the message
				QStringList otherSig = m_dbc->getMessage(msgs.at(i))->getSignalList();
				for(int j = 0; j < otherSig.size(); j++)
				{
					// Check if any other signals are in the registered list
					if(m_sig_list.contains(otherSig.at(j)))
						keepMessage = true;
				}

				if(!keepMessage)
					m_msg_list.removeOne(msgs.at(i));
			}
		}

		m_sig_list.remove(sigName);
	}
}

void CanDecoder::unregisterAllSignals()
{
	m_sig_list.clear();
}

void CanDecoder::newCanMessageReceived(CanTraceMessage trace_msg)
{
	if(m_dbc == 0)
		return;

	if(m_msg_list.contains(trace_msg.id)) // Message registered
	{
		// Propagate signal
		CanMessage *msg = m_dbc->getMessage(trace_msg.id);

		if(msg == NULL)
			return;

		QList<SignalValue_t>sig_list = msg->getSignalsValue(trace_msg.data);

		for(int i = 0; i < sig_list.size(); i++)
		{
			emit signalValueChanged(sig_list.at(i)); // Emit signal change for all signals of this message
			// NOTE : could maybe be faster to filter the registered signals not to send useless signals update
			//qDebug() << sig_list.at(i).name << " : " << sig_list.at(i).valueStr;
		}
	}
}

SignalValue_t CanDecoder::decodeSignal(QString sigName, const CanTraceMessage& message)
{
	SignalValue_t val;

	if(m_dbc == 0)
		return val;

	CanMessage *msg = m_dbc->getMessage(message.id);

	if(msg == NULL)
		return val;

	QList<SignalValue_t>sig_list = msg->getSignalsValue(message.data);

	for(int i = 0; i < sig_list.size(); i++)
	{
		if(sig_list.at(i).name == sigName)
			return sig_list.at(i);
	}

	return val;
}

QPolygonF CanDecoder::getSignalPoints(QString sigName, const QList<CanTraceMessage> &messages)
{
	QPolygonF points;

	if(m_dbc == 0)
		return points;

	CanSignal *sig = m_dbc->getSignal(sigName);

	if(sig == NULL)
		return points;

	QList<quint32>msg_ids = sig->getAssociatedMsgIDs();

	for(int i = 0; i < messages.count(); i++)
	{
		if(!msg_ids.contains(messages.at(i).id))
			continue; // Signal not in this message

		SignalValue_t val = decodeSignal(sigName, messages.at(i));

		if(!val.name.isEmpty())
			points.append(QPointF(messages.at(i).timestamp, val.value));
	}

	return points;
}

/*
QList<SignalValue_t> CanDecoder::decodeSignals(QString sigName, const QList<CanTraceMessage>& messages)
{
	QList<SignalValue_t> list;

	if(m_dbc == 0)
		return list;

	CanSignal *sig = m_dbc->getSignal(sigName);

	if(sig == NULL)
		return list;

	QList<quint32>msg_ids = sig->getAssociatedMsgIDs();

	for(int i = 0; i < messages.count(); i++)
	{
		if(!msg_ids.contains(messages.at(i).id))
			continue; // Signal not in this message

		SignalValue_t val = decodeSignal(sigName, messages.at(i));

		if(!val.name.isEmpty())
			list.append(val);
	}

	return list;
}
*/


