#ifndef CANDECODER_H
#define CANDECODER_H

#include "struct_dbc.h"
#include "struct_canmessage.h"
#include "struct_cansignal.h"
#include "model_cantrace.h"

class CanDecoder : public QObject
{
	Q_OBJECT

	public:
		CanDecoder(DbcModel *dbc, QObject *parent = 0);

		bool registerSignal(QString sigName);
		void unregisterSignal(QString sigName);
		void unregisterAllSignals();
		void setDbc(DbcModel *dbc);

		DbcModel* getDbc();

	public slots:
		void newCanMessageReceived(CanTraceMessage trace_msg);

	signals:
		void signalValueChanged(SignalValue_t);

	private:
		QList<quint32> m_msg_list; // List of message registered
		DbcModel *m_dbc;
		QHash<QString, int> m_sig_list;	// List of registered signals and number of time signal is registered

};

#endif // CANDECODER_H
