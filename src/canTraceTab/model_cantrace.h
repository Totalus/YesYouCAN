#ifndef CANTRACEMODEL_H
#define CANTRACEMODEL_H

#include <QtWidgets>
#include "struct_dbc.h"

// Columns order
#define TRACE_COL_NUMBER		0
#define TRACE_COL_TIMESTAMP		1
#define TRACE_COL_TYPE			2
#define TRACE_COL_ID			3
#define TRACE_COL_DATA_LEN		4
#define TRACE_COL_DATA			5
#define TRACE_NB_COLUMNS		6


struct CanTraceMessage
{
	double timestamp; // in seconds
	quint32 number;	// Message number
	QString type; // Rx, Tx, Other
	quint32 id;
	QList<quint8> data;
	bool rtr;	// Request transmit flag
};


class CanTraceModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		CanTraceModel(QObject *parent = 0);

		bool loadTraceFile_PCAN(QString file_name);
		bool loadTraceFile_BusMaster(QString file_name);
		bool loadTraceFile_WiresharkExport(QString file_name);
		QStringList getErrors();
		QString getFilePath();
		void setDbcModel(DbcModel *dbc);
		void clearDbcModel();
		bool hasDbc() const;
		CanTraceMessage getMessage(int row);
		int count();
		void setCurrentSelectedMsgId(quint32 msg_id); // Called when the message selected changes

		// From QAbstractTableModel
		int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	public slots:
		void addMessage(const CanTraceMessage & msg); // Called when a new message is received
		void refreshViews(); // Emit model change signal for views to refresh

		void useTimestampOffset(bool enable);
		void useMessageNumberOffset(bool enable);

	private:
		DbcModel* m_dbc;
		QList<CanTraceMessage> m_messages;
		QString m_trace_file;
		QStringList m_errors;
		quint32 currentSelectedMsgId;
		QTimer *m_timer;
		int m_nb_msg;	// Number of messages in the model since last refreshViews function call

		bool m_offsetTimestamp;
		bool m_offsetMsgNumber;

		// Private methods
		CanTraceMessage parseTraceLine_PCAN(const QString &line_, const int file_version);
		CanTraceMessage parseTraceLine_BusMaster(const QString &line_);
		CanTraceMessage parseTraceLine_WiresharkExport(const QString &line_);
};

#endif // CANTRACEMODEL_H
