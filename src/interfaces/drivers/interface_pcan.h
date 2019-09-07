#ifndef PCANINTERFACE_H
#define PCANINTERFACE_H

#include "pcandriver.h" // PCAN object (linked to dll)

#include "interface_hwinterface.h"
#include "model_cantrace.h"

class PCanInterface : public HwInterface
{
	Q_OBJECT

	public:
		PCanInterface(QObject *parent = 0, QString name = QString());
		~PCanInterface();

		// Overwritten functions
		bool initialize();
		void deinitialize();
		InterfaceType_t interfaceType();
		QString interfaceTypeStr();
		QString getDescription();
		quint32 getMessageCount();
		bool setCapture(bool enable);
		bool isCapturing();
		QString getErrorStr();
		bool writeSupported();
		void writeMessage(CanTraceMessage &msg);


		// Configuration parameters
		QList<QString> getSupportedBitrates();
		void setBitRate(QString rate);
		QString getBitRate();
		void setReadOnly(bool read_only);
		bool getReadOnly();
		void setChannel(QString channel);
		QString getChannel();

		static QStringList getAvailableDeviceList();

		QString getBusStatus();

		// QThread overloaded functions
		//void run(); // Checks for received messages

	signals:
		//void messageReceived(CanTraceMessage & msg);

	public slots:
		//void sendMessage(CanTraceMessage & msg);
		void checkForReceivedMessages(); // Get the message and emits the message received signal
		void computeRxRate();

	private:
		PCANDriver *m_pcan;

		QString m_errorStr;
		bool m_conf_readOnly;

		quint32 m_msg_count; // Number of messages received since last capture start

		QList<QPair<QString, TPCANBaudrate> > m_baud_list;
		QMap<QString, TPCANHandle> m_handle_map;

		TPCANBaudrate m_conf_bitrate;
		TPCANHandle m_channel;

		QTimer m_timer_read;
		QTimer m_timer_rate;

		bool m_capturing;

		double m_timestamp; // Timestamp of the interface in seconds
};

#endif // PCANINTERFACE_H
