#ifndef PCANDIALOG_H
#define PCANDIALOG_H

#include <QDialog>

#include "interface_pcan.h"

class PCanDialog : public QDialog
{
	Q_OBJECT

	public:
		PCanDialog(PCanInterface *iface, QWidget *parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void accept();
		void initialize(bool pressed);
		void updateStatus();

	private:
		PCanInterface *m_iface;

		// Widgets (parameters)
		QComboBox *m_bitrate;
		QLineEdit *m_name;
		QCheckBox *m_init;
		QComboBox *m_channel;

		// Widgets (status)
		QLabel *m_rx_rate;
		QLabel *m_identifier;
		QLabel *m_status_icon;
		QLabel *m_error;
		QLabel *m_status;
		QLabel *m_capturing;
		QLabel *m_bus_status;

		QTimer *m_status_timer;

		void setSupportedBitrate(QList<QString> list);
		void setBitrate(QString rate);
		void setChannelList(QStringList channels);
		void setChannel(QString channel);


};

#endif // PCANDIALOG_H
