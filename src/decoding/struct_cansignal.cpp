#include "struct_cansignal.h"

CanSignal::CanSignal()
{
	m_name = QString();
	m_max = 0;
	m_min = 0;
	m_len_bit = 8;
	m_units = QString();
	m_endianness = INTEL;
	m_signess = UNSIGNED;
	m_comment = QString();
	m_start_bit = 0;
	m_offset = 0;
	m_factor = 1;
}

CanSignal::CanSignal(QString name, double min, double max, quint8 len_bit, QString units, Endianness_t endianness, Signess_t signess, quint8 start_bit, double offset, double factor)
{
	m_name = name;
	m_max = max;
	m_min = min;
	m_len_bit = len_bit;
	m_units = units;
	m_endianness = endianness;
	m_signess = signess;
	m_comment = QString();
	m_start_bit = start_bit;
	m_offset = offset;
	m_factor = factor;
}


QString CanSignal::formatStr(quint64 raw_value, bool add_units)
{
	QString formatted_str = QString::number(format(raw_value));

	// Append the unit if applicable
	if (!m_units.isEmpty() && add_units)
	{
		formatted_str += " " + m_units; // value [units]
	}

	// Append the value from value table
	if (m_value_table.contains(raw_value))
	{
		formatted_str += " (" + m_value_table[raw_value] + ')';
	}

	return formatted_str;
}


double CanSignal::format(quint64 raw_value)
{
	// formatted_value = (raw signal value x factor) + offset

	double formatted_value = 0;

	if (getSigness() == UNSIGNED) // Unsigned interger
	{
		formatted_value = (raw_value * m_factor) + m_offset;
	}

	else if(getSigness() == SIGNED) // Signed interger
	{
		quint64 sign_bit_mask = (1 << getLengthBits());

		if( (sign_bit_mask & raw_value) != 0) // Negative number
		{
			//qint64 val_signed = -((1 << getLengthBits()) - raw_value);
			qint64 val_signed = -(sign_bit_mask - (raw_value & ~sign_bit_mask));
			formatted_value = (val_signed * m_factor) + m_offset;
		}
		else
		{
			formatted_value = (raw_value * m_factor) + m_offset; // Positive number
		}
	}

	else if(getSigness() == FLOAT_)
	{
		//float val_float = (float)(raw_value & 0xFFFFFFFF);

		quint32 t = (quint32)(raw_value & 0xFFFFFFFF);
		float val_float = *((float*)&t);
		formatted_value = (val_float * m_factor) + m_offset;
	}

	else if(getSigness() == DOUBLE_)
	{
		double val_double = *((double*)(&raw_value));
		formatted_value = (val_double * m_factor) + m_offset;
	}

	return formatted_value;
}

quint64 CanSignal::getRawValue(QString physicalValue, bool *ok)
{
	// raw signal value = (formatted_value - offset) / factor

	quint64 raw_value = 0;
	double physical = physicalValue.toDouble(ok);

	if(!*ok)
		return 0;

	if (getSigness() == UNSIGNED) // Unsigned interger
	{
		raw_value = (quint64)((physical - m_offset) / m_factor);
		//formatted_value = (raw_value * m_factor) + m_offset;
	}

	else if(getSigness() == SIGNED) // Signed interger
	{
		qint64 v = qint64((physical - m_offset) / m_factor);

		if(v >= 0)
		{
			// Positive
			raw_value = (quint64)v;
		}
		else
		{
			// Negative

			//raw_value = ((1 << getLengthBits()) - v);
			raw_value = ~v + 1;
		}
	}

	else if(getSigness() == FLOAT_)
	{
		double d = (physical - m_offset) / m_factor;
		float f = (float)d;
		quint32 raw = *((quint32*)&f);
		raw_value = (quint64)raw;
	}

	else if(getSigness() == DOUBLE_)
	{
		double d = (physical - m_offset) / m_factor;
		raw_value = *((quint64*)&d); // to uint64
	}

	return raw_value;
}


//
// Getters
//

QString CanSignal::getUnits()
{
	return m_units;
}

double CanSignal::getMax()
{
	return m_max;
}

double CanSignal::getMin()
{
	return m_min;
}

quint8 CanSignal::getLengthBits()
{
	return m_len_bit;
}

Endianness_t CanSignal::getEndianness()
{
	return m_endianness;
}

Signess_t CanSignal::getSigness()
{
	return m_signess;
}

QString CanSignal::getComment()
{
	return m_comment;
}

// Setters
void CanSignal::setUnits(QString units)
{
	m_units = units;
}

void CanSignal::setMax(double max)
{
	m_max = max;
}

void CanSignal::setMin(double min)
{
	m_min = min;
}

void CanSignal::setLengthBits(quint8 len)
{
	m_len_bit = len;
}

void CanSignal::setEndianness(Endianness_t endianness)
{
	m_endianness = endianness;
}

void CanSignal::setSigness(Signess_t signess)
{
	m_signess = signess;
}

void CanSignal::setComment(QString comment)
{
	m_comment = comment;
}

quint8 CanSignal::getStartBit()
{
	return m_start_bit;
}

QString CanSignal::getName()
{
	return m_name;
}

QList<QPair<QString, QString> > CanSignal::getParameterList()
{
	QList<QPair<QString, QString> > param_list;

	param_list.append(QPair<QString, QString>("Name",getName()));
	param_list.append(QPair<QString, QString>("Start bit",QString::number(getStartBit())));
	param_list.append(QPair<QString, QString>("Length (bits)",QString::number(getLengthBits())));


	QString str;
	if( getEndianness() == INTEL)
		str = "Intel (Little endian)";
	else if (getEndianness() == MOTO)
		str = "Motorola (Big endian)";


	param_list.append(QPair<QString, QString>("Endianness",str));

	if(getSigness() == SIGNED)
		str = "Signed";
	else if(getSigness() == UNSIGNED)
		str = "Unsigned";
	else if(getSigness() == FLOAT_)
		str = "Float";
	else if(getSigness() == DOUBLE_)
		str = "Double";

	param_list.append(QPair<QString, QString>("Signess",str));


	param_list.append(QPair<QString, QString>("Max value",QString::number(getMax())));
	param_list.append(QPair<QString, QString>("Min value",QString::number(getMin())));
	param_list.append(QPair<QString, QString>("Units",getUnits()));
	param_list.append(QPair<QString, QString>("Comment",getComment()));


	return param_list;
}

void CanSignal::setValueTable(QHash<quint64, QString> valTable)
{
	m_value_table = valTable;
}

QList<quint32> CanSignal::getAssociatedMsgIDs()
{
	return m_message_list;
}

void CanSignal::addRelatedMessage(quint32 msg_id)
{
	m_message_list.append(msg_id);
}
