#include "dialog_interfaceconfigdialog.h"

InterfaceConfigDialog::InterfaceConfigDialog(InterfaceType_t itype, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_itype = itype;

	// Common configuration
	m_name = new QLineEdit(this);
	m_bitrate = new QComboBox(this);

	// WCAN
	m_ip_address = new QLineEdit(this);
	m_ip_address->setInputMask("009.009.009.009");
	m_ip_address->setText("192.168.0.101"); // Default IP address
	m_ip_address->setVisible(false);

	m_udp_port = new QLineEdit(this);
	m_udp_port->setInputMask("00000");
	m_udp_port->setText("50000");
	m_udp_port->setVisible(false);

	// PCAN
	m_readOnly = new QCheckBox(this);
	m_readOnly->setVisible(false);
	m_channel = new QComboBox(this);
	m_channel->setVisible(false);

	// Ok / Cancel buttons
	QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

	// Construct dialog depending on interface type

	QFormLayout *paramLayout = new QFormLayout();

	if(itype == INTERFACE_WCAN)
	{
		m_ip_address->setVisible(true);
		m_udp_port->setVisible(true);

		paramLayout->addRow("Name", m_name);
		paramLayout->addRow("Bitrate", m_bitrate);
		paramLayout->addRow("Module IP address", m_ip_address);
		paramLayout->addRow("UDP port", m_udp_port);
	}
	else if(itype == INTERFACE_PCAN)
	{
		m_readOnly->setVisible(true);
		m_channel->setVisible(true);

		paramLayout->addRow("Channel", m_channel);
		paramLayout->addRow("Bitrate", m_bitrate);
		paramLayout->addRow("Read Only", m_readOnly);
		paramLayout->addRow("Name", m_name);
	}


	paramLayout->addWidget(bbox);

	setLayout(paramLayout);
	setWindowTitle("Configure Interface");

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar
}


QString InterfaceConfigDialog::getBitrate()
{
	return m_bitrate->currentText();
}

QHostAddress InterfaceConfigDialog::getHostAddress()
{
	return QHostAddress(m_ip_address->text());
}

int InterfaceConfigDialog::getPort()
{
	return m_udp_port->text().toInt();
}

void InterfaceConfigDialog::setBitrate(QString rate)
{
	for(int i = 0; i < m_bitrate->count(); i++)
	{
		if(m_bitrate->itemText(i) == rate)
		{
			m_bitrate->setCurrentIndex(i);
			break;
		}
	}
}

void InterfaceConfigDialog::setSupportedBitrate(QList<QString> list)
{
	for(int i = 0; i < list.count(); i++)
	{
		m_bitrate->addItem(list.at(i));
	}
}

void InterfaceConfigDialog::setPort(int port)
{
	m_udp_port->setText(QString::number(port));
}

void InterfaceConfigDialog::setHostAddress(QHostAddress addr)
{
	m_ip_address->setText(addr.toString());
}

void InterfaceConfigDialog::setName(QString name)
{
	m_name->setText(name);
}

QString InterfaceConfigDialog::getName()
{
	return m_name->text();
}

void InterfaceConfigDialog::accept()
{
	if(m_itype == INTERFACE_PCAN && m_channel->count() == 0)
	{
		QMessageBox::warning(this, "Invalid channel", "No PCAN device detected.");
		return;
	}

	if(m_name->text().isEmpty())
	{
		QMessageBox::warning(this, "Name missing", "The interface needs a name (ex : Robert, Steve, ...)");
		return;
	}


	QDialog::accept();
}

void InterfaceConfigDialog::setChannelList(QStringList channels)
{
	for(int i = 0; i < channels.count(); i++)
	{
		m_channel->addItems(channels);
	}
}

QString InterfaceConfigDialog::getChannel()
{
	return m_channel->currentText();
}

void InterfaceConfigDialog::setChannel(QString channel)
{
	for(int i = 0; i < m_channel->count(); i++)
	{
		if(m_channel->itemText(i) == channel)
		{
			m_channel->setCurrentIndex(i);
			return;
		}
	}
}
