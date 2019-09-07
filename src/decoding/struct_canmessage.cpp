#include "struct_canmessage.h"


CanMessage::CanMessage(QString name, quint32 id, quint8 len)
{
	m_name = name;
	m_comment = QString();
	m_id = id;
	m_len = len;
}

void CanMessage::setName(QString name)
{
	m_name = name;
}

void CanMessage::setComment(QString comment)
{
	m_comment = comment;
}

void CanMessage::setType(MessageType_t type)
{
	m_type = type;
}

quint32 CanMessage::getId()
{
	return m_id;
}

void CanMessage::appendSignal(CanSignal* sig)
{
	m_signal_list.append(QPair<CanSignal*, quint8>(sig,sig->getStartBit()));
}

QList< QPair<CanSignal*, quint8> > CanMessage::getSignals()
{
	return m_signal_list;
}

QString CanMessage::getName()
{
	return m_name;
}

quint8 CanMessage::getDataLenght()
{
	return m_len;
}

QString CanMessage::getComment()
{
	return m_comment;
}


QList< QPair<QString, QString> > CanMessage::getParameterList()
{
	QList< QPair<QString, QString> > param_list;

	param_list.append(QPair<QString, QString>("Name",getName()));
	param_list.append(QPair<QString, QString>("Message ID","0x" + QString::number(getId(),16)));
	param_list.append(QPair<QString, QString>("Data Length (bytes)",QString::number(getDataLenght())));
	param_list.append(QPair<QString, QString>("Comment",getComment()));

	return param_list;
}

quint64 CanMessage::getRawSignalValue(QString sig_name, QList<quint8> data, bool *valOk)
{
	*valOk = false;

	if(data.length() == 0)
		return 0;


	QPair<CanSignal*, quint8>pair;

	// Find pair for the specified signal
	for(int i = 0; i < m_signal_list.count(); i++)
	{
		if(m_signal_list.at(i).first->getName() == sig_name)
		{
			 pair = m_signal_list.at(i);
			 break;
		}
	}

	CanSignal* sig = pair.first;
	quint8 start_bit = pair.second;
	quint64 raw_value = 0;

	if(sig->getLengthBits() == 0)
		return 1;

	QList<quint8> value;
	quint64 mask = 0;
	quint8 shift;

	if (sig->getEndianness() == INTEL) // Little endian
	{
		quint8 start_byte = start_bit / 8;

		quint8 stop_byte = (start_bit + sig->getLengthBits() - 1) / 8;

		if(data.length() < stop_byte + 1)
			return 0;  // Not enough bytes for this signal

		shift = start_bit % 8;

		for(int k = start_byte; k <= stop_byte; k++)
		{
			value.push_back(data.at(k));
		}

		for (int j = 0; j < sig->getLengthBits(); j++)
		{
			mask |= 0x01 << j;
		}
		mask = mask << shift;
	}
	else // Big endian
	{
		//quint8 start_byte = (start_bit + 8 - sig->getLengthBits()) / 8;
		//quint8 stop_byte = (start_bit + 1) / 8;
		quint8 start_byte = (start_bit / 8);
		quint8 stop_byte = (start_bit + sig->getLengthBits() - 8)/8;

		if(data.length() < stop_byte + 1)
			return 0; // Not enough bytes for this signal

		shift = 7 - (sig->getLengthBits() - (8 - start_bit % 8)) % 8;

		//qDebug() << sig->getLengthBits() << start_bit << stop_byte << start_byte;
		for(int k = start_byte; k <= stop_byte; k++)
		{
			//if(k >= data.length())
			//	qDebug() << "About to fail ";

			value.push_front(data.at(k));
		}

		for (int j = 0; j < sig->getLengthBits(); j++)
		{
			mask |= 0x01 << j;
		}
		mask = mask << shift;
	}

	//qDebug() << mask << raw_value << shift << value << sig->getEndianness();

	// Cast the value into a 64 bits uint
	for(int n = 0; n < value.length(); n++)
	{
		raw_value += value.at(n) << (8*n);
	}
	raw_value = (raw_value & mask) >> shift;

	*valOk = true;
	return raw_value;
}

QList<SignalValue_t> CanMessage::getSignalsValue(QList<quint8> data)
{
	QList<SignalValue_t> list;

	for(int i = 0; i < m_signal_list.length(); i++)
	{
		QPair<CanSignal*, quint8>pair = m_signal_list.at(i);
		CanSignal* sig = pair.first;
		bool ok;

		quint64 raw_value = getRawSignalValue(sig->getName(), data, &ok);

		if(ok)
		{
			SignalValue_t val;
			val.name = sig->getName();
			val.valueStr = sig->formatStr(raw_value, false);
			val.unit = sig->getUnits();
			val.rawValue = raw_value;

			val.value = sig->format(raw_value);
			val.max = sig->getMax();
			val.min = sig->getMin();

			list.append(val);
		}
		else // Raw value not valid
		{
			SignalValue_t val;
			if(raw_value == 0)
			{
				val.name = sig->getName();
				val.valueStr = "Not enough data bytes for this signal";
				val.unit = "";
				val.rawValue = 0;
				val.value = 0;

				val.max = sig->getMax();
				val.min = sig->getMin();
			}
			list.append(val);
		}
	}

	return list;
}
/*
QList<QPair<QString, QString> > CanMessage::formatSignals(QList<quint8> data)
{
	QList<QPair<QString, QString> > list;

	if(data.length() > getDataLenght())
		list << QPair<QString,QString>("WARNING","Data length of this message is not consistent with the length specified in the dbc file (more byte than expected)");

	for(int i = 0; i < m_signal_list.length(); i++)
	{
		QPair<CanSignal*, quint8>pair = m_signal_list.at(i);
		CanSignal* sig = pair.first;
		bool ok;

		quint64 raw_value = getRawSignalValue(sig->getName(), data, &ok);

		if(ok)
		{
			list.append(QPair<QString,QString>(sig->getName(), sig->format(raw_value, true)));
		}
		else // Raw value not valid
		{
			if(raw_value == 0)
				list << QPair<QString,QString>(sig->getName(),"Not enough data bytes for this signal (data length of this message is not consistent with the length specified in the dbc file)");
		}
	}

	return list;

}
*/

CanSignal* CanMessage::getSignal(QString sig_name)
{
	for(int i = 0; i < m_signal_list.length(); i++)
	{
		if(m_signal_list.at(i).first->getName() == sig_name)
			return m_signal_list.at(i).first;
	}

	return 0;
}


//
// Returns the list of names of signals in this message
//
QStringList CanMessage::getSignalList()
{
	QStringList list;

	for(int i = 0; i < m_signal_list.size() ; i++)
	{
		list.append(m_signal_list.at(i).first->getName());
	}

	return list;
}

