#include "dialog_wcandialog.h"

WCANDialog::WCANDialog(WCANInterface *iface, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_iface = iface;

	m_identifier = new QLabel(this);

	m_name = new QLineEdit(this);
	m_bitrate = new QComboBox(this);

	m_ip_address = new QLineEdit(this);
	m_ip_address->setInputMask("009.009.009.009");

	m_udp_port = new QLineEdit(this);
	m_udp_port->setInputMask("00000");

	m_enable_capture = new QCheckBox("Enable capture", this);

	m_status_icon = new QLabel(this);
	m_status_icon->setFixedSize(15,15);

	QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

	QFormLayout *paramLayout = new QFormLayout();
	paramLayout->addRow(m_identifier);
	paramLayout->addRow("Name", m_name);
	paramLayout->addRow("Bitrate", m_bitrate);
	paramLayout->addRow("Host IP address", m_ip_address);
	paramLayout->addRow("UDP port", m_udp_port);
	paramLayout->addRow("Status", m_status_icon);
	paramLayout->addRow(m_enable_capture);
	paramLayout->addWidget(bbox);

	QPixmap pix("D:\\Vbox_Partage\\eRacing_NotVersioned\\YesYouCAN\\tesla.png");
	m_status_icon->setPixmap(pix);

	// Fill information
	m_identifier->setText("Wireless CAN interface (WeCAN)");
	m_ip_address->setText(iface->getHostAddress().toString());
	m_udp_port->setText(QString::number(iface->getPort()));
	setSupportedBitrate(iface->getSupportedBitrates());
	setBitrate(iface->getBitRate());
	m_name->setText(iface->getName());

	setLayout(paramLayout);

	setWindowTitle("Configure Interface");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar
}

QHostAddress WCANDialog::getHostAddress()
{
	return QHostAddress(m_ip_address->text());
}

int WCANDialog::getPort()
{
	return m_udp_port->text().toInt();
}

void WCANDialog::setBitrate(QString rate)
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

void WCANDialog::setSupportedBitrate(QList<QString> list)
{
	for(int i = 0; i < list.count(); i++)
	{
		m_bitrate->addItem(list.at(i));
	}
}

void WCANDialog::accept()
{
	// Apply changes
	applyConfiguration();

	QDialog::accept();
}

void WCANDialog::applyConfiguration()
{
	// Write configuration to interface

	m_iface->setName(m_name->text());
	m_iface->setIdentifier(m_name->text());
	m_iface->setBitRate(m_bitrate->currentText());
	m_iface->setPort(m_udp_port->text().toInt());
	m_iface->setHostAddress(QHostAddress(m_ip_address->text()));
}



