#include "dialog_pcandialog.h"

PCanDialog::PCanDialog(PCanInterface *iface, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_iface = iface;

	// Widgets
	//m_identifier = new QLabel(this);
	m_bitrate = new QComboBox(this);
	m_name = new QLineEdit(this);

	m_init = new QCheckBox("Initialize", this);
	connect(m_init, SIGNAL(clicked(bool)), this, SLOT(initialize(bool)));
	m_channel = new QComboBox(this);

	m_status_icon = new QLabel(this);
	m_rx_rate = new QLabel(this);
	m_error = new QLabel(this);
	m_status = new QLabel(this);
	m_capturing = new QLabel(this);
	m_bus_status = new QLabel(this);

	// Dialog buttons
	QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

	// Parameter inputs layout
	QFormLayout *paramLayout = new QFormLayout();
	paramLayout->addRow("Channel", m_channel);
	paramLayout->addRow("Bitrate", m_bitrate);
	paramLayout->addRow(m_init);
	paramLayout->addRow("Name", m_name);
	paramLayout->addWidget(bbox);

	QGroupBox *param_box = new QGroupBox("Parameters", this);
	param_box->setLayout(paramLayout);

	// Status layout
	QFormLayout *statusLayout = new QFormLayout();
	//statusLayout->addRow(m_identifier);
	//statusLayout->addRow(m_status_icon);
	statusLayout->addRow("State : ", m_status);
	statusLayout->addRow("Capture : ", m_capturing);
	statusLayout->addRow("Rx rate (msg/s) : ", m_rx_rate);
	statusLayout->addRow("Bus Status : ", m_bus_status);
	statusLayout->addRow(m_error);

	QGroupBox *status_box = new QGroupBox("Status", this);
	status_box->setLayout(statusLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(param_box);
	layout->addWidget(status_box);

	// Fill information
	//m_identifier->setText("PEAK PCAN interface");
	setSupportedBitrate(iface->getSupportedBitrates());
	setBitrate(iface->getBitRate());
	m_name->setText(iface->getName());
	m_init->setChecked(iface->getState() == INITIALIZED);

	if(iface->getChannel().isEmpty())
	{
		qDebug() << m_iface->getAvailableDeviceList();
		setChannelList(m_iface->getAvailableDeviceList());
	}
	else
	{
		m_channel->addItem(iface->getChannel());
		m_channel->setDisabled(true);
	}

	setLayout(layout);

	setWindowTitle("PCAN interface configuration");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar

	updateStatus();

	// Auto status update
	m_status_timer = new QTimer(this);
	m_status_timer->setInterval(300);
	connect(m_status_timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	//connect(this, SIGNAL(finished(int)), m_status_timer, SLOT(stop()), Qt::QueuedConnection);
	m_status_timer->start();
}

void PCanDialog::accept()
{
	qDebug() << QThread::currentThreadId();
	if(m_channel->currentText().isEmpty())
	{
		QMessageBox::warning(this, "Invalid channel", "No channel selected or no PCAN device detected.");
		return;
	}

	m_iface->setChannel(m_channel->currentText());
	m_iface->setBitRate(m_bitrate->currentText());

	//m_status_timer->stop();

	QDialog::accept();
}

void PCanDialog::setBitrate(QString rate)
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

void PCanDialog::setSupportedBitrate(QList<QString> list)
{
	for(int i = 0; i < list.count(); i++)
	{
		m_bitrate->addItem(list.at(i));
	}
}

void PCanDialog::setChannelList(QStringList channels)
{
	m_channel->addItems(channels);
}

void PCanDialog::setChannel(QString channel)
{
	for(int i = 0; i < m_channel->count(); i++)
	{
		if(m_channel->itemText(i) == channel)
		{
			m_channel->setCurrentIndex(i);
			break;
		}
	}
}

void PCanDialog::initialize(bool pressed)
{
	if(pressed)
	{
		// Initialize

		if(m_channel->currentText().isEmpty())
		{
			QMessageBox::warning(this, "Invalid channel", "No channel selected or no PCAN device detected.");
			return;
		}

		m_iface->setChannel(m_channel->currentText());
		m_iface->setBitRate(m_bitrate->currentText());

		if(m_iface->initialize())
		{
			// Initialize success
			m_channel->setEnabled(false);
			m_bitrate->setEnabled(false);
		}
		else
			m_init->setChecked(false);
	}
	else
	{
		// Deinitialize
		m_iface->deinitialize();

		m_channel->setEnabled(true);
		m_bitrate->setEnabled(true);
	}
}

void PCanDialog::updateStatus()
{
	m_rx_rate->setText(QString::number(m_iface->getRxRate()));

	if(!m_iface->getErrorStr().isEmpty())
		m_error->setText("Last Error : " + m_iface->getErrorStr());
	else
		m_error->setText("");

	switch(m_iface->getState())
	{
		case OFFLINE:
			m_status->setText("Not Initialized");
			break;

		case INITIALIZED:
			m_status->setText("Initialized");
			break;

		case UNAVAILABLE:
			m_status->setText("Unavailable");
			break;

		default:
			m_status->setText("Unknown");
	}

	m_capturing->setText(m_iface->isCapturing() ? "ON" : "OFF");

	m_bus_status->setText(m_iface->getBusStatus());
}
