#ifndef CANSIGNAL_H
#define CANSIGNAL_H

#include <QtCore>
#include "struct_canmessage.h"

class CanMessage;

typedef enum {
	INTEL = 0,  // Little endian
	MOTO = 1,   // Big endian
} Endianness_t;


typedef enum{
	SIGNED,
	UNSIGNED,
    FLOAT_,
    DOUBLE_
} Signess_t;



class CanSignal
{
	public:
		CanSignal();
		CanSignal(QString name, double min, double max, quint8 len_bit, QString units, Endianness_t endianness, Signess_t signess, quint8 start_bit, double offset, double factor);
		QString formatStr(quint64 value, bool add_units = false); // Fromat for display
		double format(quint64); // Physical value as a double
		QList< QPair<QString, QString> > getParameterList(); // Send the list of parameters and value for that signal
		quint64 getRawValue(QString physicalValue, bool *ok = 0);	// From physical value, get the raw value

		// Getters
		QString getUnits();
		double getMax();
		double getMin();
		quint8 getLengthBits();
		Endianness_t getEndianness();
		Signess_t getSigness();
		QString getComment();
		quint8 getStartBit();
		QString getName();
		QList<quint32> getAssociatedMsgIDs();

		// Setters
		void setUnits(QString units);
		void setMax(double max);
		void setMin(double min);
		void setLengthBits(quint8 len);
		void setEndianness(Endianness_t endianness);
		void setSigness(Signess_t signess);
		void setComment(QString comment);
		void setValueTable(QHash<quint64, QString> valTable);
		void addRelatedMessage(quint32 msg_id);

	private:
		QString m_name;   // signal name
		double m_max;
		double m_min;
		double m_factor;
		double m_offset;
		quint8 m_start_bit;
		quint8 m_len_bit;
		QString m_units;
		Endianness_t m_endianness;	 // Little endian (Intel) vs Big endian (Motorolla)
		Signess_t m_signess;	   // Signed (1) vs Unsigned (0)
		QString m_comment;
		QList<quint32> m_message_list; // Message list
		QHash<quint64, QString> m_value_table; // Value table

};

#endif // CANSIGNAL_H
