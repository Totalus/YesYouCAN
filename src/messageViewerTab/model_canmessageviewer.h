#ifndef CANFIXEDTRACEMODEL_H
#define CANFIXEDTRACEMODEL_H

#include <QtWidgets>
#include "model_cantrace.h"

// Columns order
#define FTRACE_COL_TYPE			0
#define FTRACE_COL_ID			1
#define FTRACE_COL_DATA_LEN		2
#define FTRACE_COL_DATA			3
#define FTRACE_COL_COUNT		4
#define FTRACE_COL_PERIOD		5
#define FTRACE_COL_SIGNALS		6
#define FTRACE_NB_COLUMNS		7

struct CanFixedTraceMessage
{
	CanTraceMessage last_msg;
	double timestamp_previous;	// Time stamp before timestamp_old
	int period_ms;
	int count;
	bool disp_signals;
};


class CanMessageViewerModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		CanMessageViewerModel(QObject *parent = 0);

		QStringList getErrors();
		void setDbcModel(DbcModel *dbc);
		void clearDbcModel();
		bool hasDbc() const;
		int count();

		// From QAbstractTableModel
		int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
		bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
		//QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
		//QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
		//bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE;


	public slots:
		void newMessage(CanTraceMessage msg);
		void refreshViews();
		void resetData();

	private:
		QMap<quint32, CanFixedTraceMessage> m_messages;
		DbcModel* m_dbc;
		QStringList m_errors;
		int m_nb_msg; // Number of messages since last refresh of view call (refreshViews call)

		QTimer *m_timer;
};

#endif // CANFIXEDTRACEMODEL_H


