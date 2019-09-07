#ifndef MODEL_CANTRANSMIT_H
#define MODEL_CANTRANSMIT_H

#include "model_cantrace.h"
#include "interface_hwinterface.h"

// Columns order
#define TX_COL_ID			0
#define TX_COL_DATA_LEN		1
#define TX_COL_DATA			2
#define TX_COL_COUNT		3
#define TX_COL_PERIOD		4
#define TX_COL_SEND			5
#define TX_COL_COMMENT		6
#define TX_NB_COLUMNS		7


class CanTxMessageObject : public QObject
{
	Q_OBJECT

	public: // Functions
		CanTxMessageObject(QObject *parent = 0) : QObject(parent)
		{
			m_timer.setTimerType(Qt::PreciseTimer);
			connect(&m_timer, SIGNAL(timeout()), this, SLOT(sendMessageSlot()));
			this->m_count = 0;

			m_traceMsg.id = 0;
			m_traceMsg.type = "Tx";
		}

		void setCanMessage(CanTraceMessage msg)
		{
			m_traceMsg = msg;
		}

		void setTransmit(bool enable)	// Enable / Disable message transmit
		{
			if(enable && m_timer.interval() > 0)
				m_timer.start();
			else
				m_timer.stop();
		}

		bool getTransmit()
		{
			return m_timer.isActive();
		}

		void setPeriod(int period_ms)
		{
			if(period_ms <= 0)
				setTransmit(false); // Disable transmission (manual mode)

			m_timer.setInterval(period_ms);
		}

		void setComment(QString comment)
		{
			m_comment = comment;
		}

		QString getComment()
		{
			return m_comment;
		}

		int getPeriod()
		{
			return m_timer.interval();
		}

		quint32 getCount()
		{
			return m_count;
		}

		CanTraceMessage getCanMessage()
		{
			return m_traceMsg;
		}

	signals:
		void sendMessage(CanTraceMessage &trace_msg);

	public slots:
		void sendMessageSlot()
		{
			emit sendMessage(m_traceMsg);
			m_count++;
		}

	private:
		CanTraceMessage m_traceMsg;
		QString m_comment;
		quint32 m_count;
		QTimer m_timer;
};

class CanTransmitterModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		CanTransmitterModel(QObject *parent = 0);

		QStringList getErrors();
		void setDbcModel(DbcModel *dbc);
		void clearDbcModel();
		bool hasDbc() const;
		int count();

		void addCanMessage(CanTxMessageObject* msgObj);
		CanTxMessageObject* getCanMessage(int row);
		void removeMessage(int row);
		void setDevice(HwInterface *iface);

		// From QAbstractTableModel
		int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
		bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;

	public slots:
		//void refreshViews();
		void updateMessageCount();

	private:
		QList<CanTxMessageObject *> m_messagesObjects;
		DbcModel* m_dbc;
		QStringList m_errors;

		HwInterface *m_device;

		QTimer *m_timer;
};

#endif // MODEL_CANTRANSMIT_H
