#ifndef INTERFACECONFIGDIALOG_H
#define INTERFACECONFIGDIALOG_H

#include <QtWidgets>
#include <QHostAddress>
#include "interface_hwinterface.h"

class InterfaceConfigDialog : public QDialog
{
	Q_OBJECT

	public:
		InterfaceConfigDialog(InterfaceType_t itype, QWidget *parent = 0, Qt::WindowFlags f = 0);

		// Common
		QString getBitrate();
		QString getName();

		void setSupportedBitrate(QList<QString> list);	// rates in bits/s
		void setBitrate(QString rate);			// rate in bits/s
		void setName(QString name);

		// WCAN
		QHostAddress getHostAddress();
		int getPort();
		void setPort(int port);
		void setHostAddress(QHostAddress addr);

		// PCAN
		void setChannelList(QStringList channels);
		void setChannel(QString channel);
		QString getChannel();


	public slots:
		void accept();

	private:
		QComboBox *m_bitrate;
		QLineEdit *m_name;

		// WCAN
		QLineEdit *m_ip_address;
		QLineEdit *m_udp_port;

		// PCAN
		QComboBox *m_channel;

		QCheckBox *m_readOnly;

		InterfaceType_t m_itype;
};

#endif // INTERFACECONFIGDIALOG_H
