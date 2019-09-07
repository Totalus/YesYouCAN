#ifndef WCANDIALOG_H
#define WCANDIALOG_H

#include <QtWidgets>
#include <QtNetwork/QHostAddress>
#include "interface_wcan.h"


class WCANDialog : public QDialog
{
	public:
		WCANDialog(WCANInterface *iface, QWidget *parent = 0, Qt::WindowFlags f = 0);
		QHostAddress getHostAddress();
		int getPort();

		void applyConfiguration();

		// From QDialog
		void accept();

	private:
		WCANInterface *m_iface;

		// Widgets
		QLabel *m_identifier;
		QComboBox *m_bitrate;
		QLineEdit *m_name;
		QLineEdit *m_ip_address;
		QLineEdit *m_udp_port;
		QCheckBox *m_enable_capture;
		QLabel *m_status_icon;


		void setSupportedBitrate(QList<QString> list);
		void setBitrate(QString rate);
};

#endif // WCANDIALOG_H
