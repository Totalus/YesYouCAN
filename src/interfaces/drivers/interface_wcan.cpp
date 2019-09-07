#include "interface_wcan.h"

WCANInterface::WCANInterface(QHostAddress address, int port, QObject *parent, QString name) : HwInterface(parent, name)
{
	m_socket = 0;
	m_port = port;
	m_hostAddress = address;
	m_msg_count = 0;
	m_msg_count_last = 0;

	m_timer = new QTimer(this);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(computeRxRate()));
	m_timer->start(1000); // ms

	m_errorStr = "";

	// Supported baud rates
	m_baud_list << QPair<QString,int>("500 kBit/s",500);
	m_baud_list << QPair<QString,int>("250 kBit/s",250);
	m_baud_list << QPair<QString,int>("125 kBit/s",125);
	m_baud_list << QPair<QString,int>("100 kBit/s",100);

	m_capabilities.clear();
	m_capabilities << "read";
}

bool WCANInterface::initialize()
{
	if(m_socket == 0) // Socket does not exist
	{
		m_socket = new QUdpSocket(this);
		QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(newMessageReceived()), Qt::UniqueConnection);
	}

	m_state = INITIALIZED;

	m_errorStr = "No Error";

	return true;
}

void WCANInterface::deinitialize()
{
	if(m_socket != 0)
		m_socket->abort();

	m_state = OFFLINE;
}

void WCANInterface::setPort(int port)
{
	m_port = port;
}

void WCANInterface::setHostAddress(QHostAddress address)
{
	m_hostAddress = address;
}

void WCANInterface::newMessageReceived()
{
	while (m_socket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(m_socket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		if(datagram.size() < 22)
			return;

		// Datagram (udp payload) :
		//	[0]			: command ID (0x0D = CMD_CAN_RX)
		//	[1..4]		: message number (4 bytes, uint32, little endian)
		//	[5..8]		: timestamp (4 bytes, uint32, ms, little endian)
		//	[9..12]		: CAN msg ID (4 bytes, little endian)
		//	[13]		: DLC (1 byte, uint8)
		//	[14..21]	: Data (8 bytes, padding de 0 si le DLC < 8)

		if(datagram.at(0) == 0x0D) // CMD_CAN_RX : interface has received a can message
		{
			CanTraceMessage msg;
			msg.number = datagram.at(4) + (datagram.at(3) << 8) + (datagram.at(2) << 16) + (datagram.at(1) << 24);
			msg.timestamp = ((quint8)datagram.at(8) + ((quint8)datagram.at(7) << 8) + ((quint8)datagram.at(6) << 16) + ((quint8)datagram.at(5) << 24))/1000.0;
			msg.id = (quint8)datagram.at(12) | ((quint8)datagram.at(11) << 8) | ((quint8)datagram.at(10) << 16) | ((quint8)datagram.at(9) << 24);
			msg.type = "Rx";

			quint8 dlc = datagram.at(13); // Data length

			if(dlc > 8) // Impossible data length
				return;

			msg.data.clear();
			for(int i = 0; i < dlc; i++)
			{
				msg.data.append(datagram.at(i+14));
			}

			// Emit signal to indicate a new CAN message was received on that interface
			emit newCanMessage(msg);

			m_msg_count++;
		}
	}
}

void WCANInterface::computeRxRate()
{
	if(m_msg_count < m_msg_count_last)
		m_msg_count_last = 0;

	m_rx_rate = 1000*(m_msg_count - m_msg_count_last)/m_timer->interval();

	m_msg_count_last = m_msg_count;
}

InterfaceType_t WCANInterface::interfaceType()
{
	return INTERFACE_WCAN;
}

QString WCANInterface::interfaceTypeStr()
{
	return "WCAN";
}

int WCANInterface::getPort()
{
	return m_port;
}

QHostAddress WCANInterface::getHostAddress()
{
	return m_hostAddress;
}


QString WCANInterface::getDescription()
{
	return "WCAN, Rate: " + getBitRate() + ", IP: " + m_hostAddress.toString() + ", UDP: " + QString::number(m_port);
}

bool WCANInterface::setCapture(bool enable)
{
	// If device not initialize, initialize it
	if( (m_state == OFFLINE) | (m_state == UNAVAILABLE) | (m_socket == 0))
	{
		if(!initialize())
		{
			QMessageBox::warning(0, "Initialization failed", "Could not initialize WCAN interface.");
			return false;
		}
	}

	// Enable or disable capture
	if(enable)
	{
		// Connect UDP socket to start capture
		if(m_socket->bind(m_hostAddress, m_port))
		{
			m_capturing = true;
			m_msg_count = 0;
		}
		else
		{
			QMessageBox::warning(0, "Connection error", "Can't open UDP socket on address " + m_hostAddress.toString() + " port " + QString::number(m_port) + " : "
								 + m_socket->errorString());
		}
	}
	else if(m_capturing)
	{
		// Disconnect UDP socket
		m_socket->abort();
		m_capturing = false;
	}

	return true;
}

bool WCANInterface::isCapturing()
{
	return m_capturing;
}

quint32 WCANInterface::getMessageCount()
{
	return m_msg_count;
}


QString WCANInterface::getErrorStr()
{
	return m_errorStr;
}

QList<QString> WCANInterface::getSupportedBitrates()
{
	QList<QString> list;

	for(int i = 0; i < m_baud_list.count(); i++)
	{
		list.append(m_baud_list.at(i).first);
	}

	return list;
}

void WCANInterface::setBitRate(QString rate)
{
	for(int i = 0; i < m_baud_list.count(); i++)
	{
		if(m_baud_list.at(i).first == rate)
		{
			// Set data rate
			m_bitrate = m_baud_list.at(i).second;
			return;
		}
	}

	qDebug() << "Wrong datarate given : " << rate;
}

QString WCANInterface::getBitRate()
{
	for(int i = 0; i < m_baud_list.count(); i++)
	{
		if(m_baud_list.at(i).second == m_bitrate)
			return m_baud_list.at(i).first;
	}

	return "";
}
