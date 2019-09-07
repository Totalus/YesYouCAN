#ifndef DBCMODEL_H
#define DBCMODEL_H

#include <QtCore>
#include "struct_canmessage.h"
#include "struct_cansignal.h"

class DbcModel
{
	public:
		DbcModel(QString file_name);
		~DbcModel();

		CanMessage* getMessage(quint32 id); // Get message from ID (fast)
		CanMessage* getMessage(const QString& name); // Get message from name (slower)
		QList<quint32> getMessagesId();
		CanSignal* getSignal(const QString& name);
		QStringList getSignalList();
		QStringList getMessageNames();
		QStringList getErrors();
		QString getFileName();
		QString getFullPath();
		bool containsSignal(const QString& signal_name);

	private:
		// Members
		QHash<quint32, CanMessage*> m_messages; // < id, message >
		QHash<QString, CanSignal*>	m_signals;
		QString						m_file_path;
		QStringList					m_errors; // Parsing errors


		// Methods
		void		loadDbcFile();
		CanMessage* create_message(QString line_);

		CanSignal*	parse_signal(QString line_);
		void		parse_comment(QString line_);
		void		parse_valtype(QString line_);
		void		parse_valtable(QString line_);
};

#endif // DBCMODEL_H
