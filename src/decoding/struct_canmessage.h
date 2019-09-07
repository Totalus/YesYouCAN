#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <QtCore>
#include "struct_cansignal.h"

class CanSignal;

typedef enum
{
	STANDARD,   // CAN standard (11 bit ID)
	EXTENDED	// CAN extended (29 bit ID)
} MessageType_t;

typedef struct
{
	QString name;		// Signal name
	QString valueStr;	// Formatted value for display (string)
	QString unit;		// Signal units
	quint64	rawValue;	// Raw value
	double max;			// Maximum
	double min;			// Minimum
	double value;		// Physical value as a double
} SignalValue_t;


class CanMessage
{
	public:
		CanMessage(QString name, quint32 id, quint8 len);

		// Format / parsing
		//QList<QPair<QString, QString> > formatSignals(QList<quint8> data); // Obsolète, remplacé par getSignalsValue
		QList<SignalValue_t> getSignalsValue(QList<quint8> data);

		quint64 getRawSignalValue(QString sig_name, QList<quint8> data, bool *ok);	// Extract raw value from message data for a signal

		// Setters
		void setName(QString name);
		void setComment(QString comment);
		void setType(MessageType_t type);
		void appendSignal(CanSignal* sig);

		// Getters
		QList< QPair<CanSignal*, quint8> > getSignals();
		QList< QPair<QString, QString> > getParameterList();
		quint32 getId();
		QString getName();
		quint8 getDataLenght();
		QString getComment();
		CanSignal* getSignal(QString sig_name);
		QStringList getSignalList();

	private:
		quint32 m_id;
		QString m_name;
		QString m_comment;
		quint8 m_len;
		MessageType_t m_type;
		QList< QPair<CanSignal*, quint8 > > m_signal_list; // QPair<signal, start bit>
		// List of start bit, lenght, name (to parse the ID)
};

#endif // CANMESSAGE_H
