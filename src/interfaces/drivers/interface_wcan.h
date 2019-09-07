#ifndef WCANINTERFACE_H
#define WCANINTERFACE_H

#include <QtNetwork>
#include "interface_hwinterface.h"
#include "dialog_interfaceconfigdialog.h"

class WCANInterface : public HwInterface
{
	Q_OBJECT

	public:
		WCANInterface(QHostAddress address, int port, QObject *parent = 0, QString name = QString());
		bool initialize();
		void deinitialize();

		// Overloaded function (from parent class)
		InterfaceType_t interfaceType();
		QString interfaceTypeStr();
		QString getDescription();
		bool setCapture(bool enable);
		quint32 getMessageCount();
		QString getErrorStr();
		bool isCapturing();

		// Setters
		void setPort(int port);
		void setHostAddress(QHostAddress address);
		void setBitRate(QString rate);

		// Getters
		QList<QString> getSupportedBitrates();
		int getPort();
		QHostAddress getHostAddress();
		QString getBitRate();

	public slots:
		void newMessageReceived();
		void computeRxRate();

	private:
		QUdpSocket *m_socket;
		int m_port;
		QHostAddress m_hostAddress;

		QTimer *m_timer;

		quint32 m_msg_count;
		quint32 m_msg_count_last;

		QString m_errorStr;

		QList<QPair<QString, int> > m_baud_list;
		int m_bitrate;

		bool m_capturing;
};

#endif // WCANINTERFACE_H
