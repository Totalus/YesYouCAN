#include "interface_pcan.h"

PCanInterface::PCanInterface(QObject *parent, QString name) : HwInterface(parent)
{
	m_name = name;
	m_identifier = "";
	m_pcan = 0;
	m_errorStr = "";
	m_conf_bitrate = PCAN_BAUD_500K; // Default 500 kbps
	m_msg_count = 0;

	m_capabilities.clear();
	m_capabilities << "write" << "read"; // TODO : verify capabilities by reading from device

	m_timer_rate.start(1000);
	connect(&m_timer_read, SIGNAL(timeout()), this, SLOT(checkForReceivedMessages()));
	connect(&m_timer_rate, SIGNAL(timeout()), this, SLOT(computeRxRate()));

	// Supported baud rates mapping
	m_baud_list << QPair<QString,TPCANBaudrate>("1 MBit/s",PCAN_BAUD_1M);
	m_baud_list << QPair<QString,TPCANBaudrate>("800 kBit/s",PCAN_BAUD_800K);
	m_baud_list << QPair<QString,TPCANBaudrate>("500 kBit/s",PCAN_BAUD_500K);
	m_baud_list << QPair<QString,TPCANBaudrate>("250 kBit/s",PCAN_BAUD_250K);
	m_baud_list << QPair<QString,TPCANBaudrate>("125 kBit/s",PCAN_BAUD_125K);
	m_baud_list << QPair<QString,TPCANBaudrate>("100 kBit/s",PCAN_BAUD_100K);
	m_baud_list << QPair<QString,TPCANBaudrate>("95.238 kBit/s",PCAN_BAUD_95K);
	m_baud_list << QPair<QString,TPCANBaudrate>("83.333 kBit/s",PCAN_BAUD_83K);
	m_baud_list << QPair<QString,TPCANBaudrate>("50 kBit/s",PCAN_BAUD_50K);
	m_baud_list << QPair<QString,TPCANBaudrate>("47.619 kBit/s",PCAN_BAUD_47K);
	m_baud_list << QPair<QString,TPCANBaudrate>("33.333 kBit/s",PCAN_BAUD_33K);
	m_baud_list << QPair<QString,TPCANBaudrate>("20 kBit/s",PCAN_BAUD_20K);
	m_baud_list << QPair<QString,TPCANBaudrate>("10 kBit/s",PCAN_BAUD_10K);
	m_baud_list << QPair<QString,TPCANBaudrate>("5 kbps",PCAN_BAUD_5K);

	// Handler value mapping (Plug and play handlers only)
	m_handle_map["PCAN_PCIBUS1"]	= PCAN_PCIBUS1;
	m_handle_map["PCAN_PCIBUS2"]	= PCAN_PCIBUS2;
	m_handle_map["PCAN_PCIBUS3"]	= PCAN_PCIBUS3;
	m_handle_map["PCAN_PCIBUS4"]	= PCAN_PCIBUS4;
	m_handle_map["PCAN_PCIBUS5"]	= PCAN_PCIBUS5;
	m_handle_map["PCAN_PCIBUS6"]	= PCAN_PCIBUS6;
	m_handle_map["PCAN_PCIBUS7"]	= PCAN_PCIBUS7;
	m_handle_map["PCAN_PCIBUS8"]	= PCAN_PCIBUS8;
	m_handle_map["PCAN_PCIBUS9"]	= PCAN_PCIBUS9;
	m_handle_map["PCAN_PCIBUS10"]	= PCAN_PCIBUS10;
	m_handle_map["PCAN_PCIBUS11"]	= PCAN_PCIBUS11;
	m_handle_map["PCAN_PCIBUS12"]	= PCAN_PCIBUS12;
	m_handle_map["PCAN_PCIBUS13"]	= PCAN_PCIBUS13;
	m_handle_map["PCAN_PCIBUS14"]	= PCAN_PCIBUS14;
	m_handle_map["PCAN_PCIBUS15"]	= PCAN_PCIBUS15;
	m_handle_map["PCAN_PCIBUS16"]	= PCAN_PCIBUS16;
	m_handle_map["PCAN_USBBUS1"]	= PCAN_USBBUS1;
	m_handle_map["PCAN_USBBUS2"]	= PCAN_USBBUS2;
	m_handle_map["PCAN_USBBUS3"]	= PCAN_USBBUS3;
	m_handle_map["PCAN_USBBUS4"]	= PCAN_USBBUS4;
	m_handle_map["PCAN_USBBUS5"]	= PCAN_USBBUS5;
	m_handle_map["PCAN_USBBUS6"]	= PCAN_USBBUS6;
	m_handle_map["PCAN_USBBUS7"]	= PCAN_USBBUS7;
	m_handle_map["PCAN_USBBUS8"]	= PCAN_USBBUS8;
	m_handle_map["PCAN_USBBUS9"]	= PCAN_USBBUS9;
	m_handle_map["PCAN_USBBUS10"]	= PCAN_USBBUS10;
	m_handle_map["PCAN_USBBUS11"]	= PCAN_USBBUS11;
	m_handle_map["PCAN_USBBUS12"]	= PCAN_USBBUS12;
	m_handle_map["PCAN_USBBUS13"]	= PCAN_USBBUS13;
	m_handle_map["PCAN_USBBUS14"]	= PCAN_USBBUS14;
	m_handle_map["PCAN_USBBUS15"]	= PCAN_USBBUS15;
	m_handle_map["PCAN_USBBUS16"]	= PCAN_USBBUS16;
	m_handle_map["PCAN_PCCBUS1"]	= PCAN_PCCBUS1;
	m_handle_map["PCAN_PCCBUS2"]	= PCAN_PCCBUS2;
	m_handle_map["PCAN_LANBUS1"]	= PCAN_LANBUS1;
	m_handle_map["PCAN_LANBUS2"]	= PCAN_LANBUS2;
	m_handle_map["PCAN_LANBUS3"]	= PCAN_LANBUS3;
	m_handle_map["PCAN_LANBUS4"]	= PCAN_LANBUS4;
	m_handle_map["PCAN_LANBUS5"]	= PCAN_LANBUS5;
	m_handle_map["PCAN_LANBUS6"]	= PCAN_LANBUS6;
	m_handle_map["PCAN_LANBUS7"]	= PCAN_LANBUS7;
	m_handle_map["PCAN_LANBUS8"]	= PCAN_LANBUS8;
	m_handle_map["PCAN_LANBUS9"]	= PCAN_LANBUS9;
	m_handle_map["PCAN_LANBUS10"]	= PCAN_LANBUS10;
	m_handle_map["PCAN_LANBUS11"]	= PCAN_LANBUS11;
	m_handle_map["PCAN_LANBUS12"]	= PCAN_LANBUS12;
	m_handle_map["PCAN_LANBUS13"]	= PCAN_LANBUS13;
	m_handle_map["PCAN_LANBUS14"]	= PCAN_LANBUS14;
	m_handle_map["PCAN_LANBUS15"]	= PCAN_LANBUS15;
	m_handle_map["PCAN_LANBUS16"]	= PCAN_LANBUS16;
}

PCanInterface::~PCanInterface()
{
	qDebug() << "~PCANInterface()";
	deinitialize();
}

bool PCanInterface::initialize()
{
	if(m_state == INITIALIZED)
	{
		qDebug() << "PCAN device already initialized";
		return true;
	}

	if(m_pcan == 0)
		m_pcan = new PCANDriver(this);

	m_pcan->load(); // Load driver (dll, functions, ...)

	if(m_pcan->getStatus() != Ready)
	{
		m_errorStr = "Could not initialize the PCAN driver : [pcan driver error]";
		return false;
	}

	// Initialize device
	TPCANStatus stsResult = m_pcan->Initialize(m_channel, m_conf_bitrate);

	if (stsResult != PCAN_ERROR_OK)
	{
		qDebug() << "PCAN initialization failed with code" << stsResult;
		m_state = UNAVAILABLE;

		if (stsResult != PCAN_ERROR_CAUTION)
		{
			// Get error str
			//stsResult = m_objPCANBasic->GetErrorText(stsResult, 0, buffer);
			//if(stsResult != PCAN_ERROR_OK)

			m_errorStr = "Could not initialize the device properly.";
			m_pcan->unload();
			return false;
		}
		else
		{
			m_errorStr = "The bitrate used is different than the given one.";
			m_pcan->Uninitialize(m_channel);
			m_pcan->unload();
			return false;
		}
	}
	qDebug() << "Initialized PCAN successfuly";

	m_state = INITIALIZED;
	return true;
}

void PCanInterface::deinitialize()
{	
	if(m_pcan == 0)
		return;

	m_state = OFFLINE;

	// Disconnect from device
	qDebug() << "Deinitialized pcan " << m_pcan->Uninitialize(m_channel);

	// Unload library
	m_pcan->unload();
}

InterfaceType_t PCanInterface::interfaceType()
{
	return INTERFACE_PCAN;
}

QString PCanInterface::interfaceTypeStr()
{
	return "PCAN-USB";
}

QString PCanInterface::getDescription()
{
	return getChannel() + " : " + getBitRate();
}

quint32 PCanInterface::getMessageCount()
{
	return m_msg_count;
}

bool PCanInterface::setCapture(bool enable)
{
	if(m_pcan == 0)
	{
		m_errorStr = "Device not initialized.";
		return false;
	}

	/*
	quint32 iBuffer = enable ? PCAN_PARAMETER_ON : PCAN_PARAMETER_OFF;
	TPCANStatus stsResult;

	stsResult = m_pcan->Reset(m_channel);
	stsResult = m_pcan->SetValue(m_channel, PCAN_RECEIVE_STATUS, (void*)&iBuffer, sizeof(iBuffer));

	if (stsResult != PCAN_ERROR_OK)
	{
		m_errorStr = "Device did not respond to the request.";
		deinitialize();
		return false;
	}
	//*/

	if(enable)
	{
		m_capturing = true;
		m_msg_count = 0;
		m_timer_read.start(10); // Launch timer which will trigger the checkForNewMessage
	}
	else
	{
		m_timer_read.stop(); // Stop timer
		m_capturing = false;
	}

	qDebug() << "PCanInterface::setCapture(" << enable << ")";

	return true;
}

bool PCanInterface::isCapturing()
{
	return m_capturing;
}

QString PCanInterface::getErrorStr()
{
	return m_errorStr;
}

QList<QString> PCanInterface::getSupportedBitrates()
{
	QList<QString> list;

	for(int i = 0; i < m_baud_list.count(); i++)
	{
		list.append(m_baud_list.at(i).first);
	}

	return list;
}

void PCanInterface::setBitRate(QString rate)
{
	for(int i = 0; i < m_baud_list.count(); i++)
	{
		if(m_baud_list.at(i).first == rate)
		{
			// Set data rate
			m_conf_bitrate = m_baud_list.at(i).second;
			return;
		}
	}

	qDebug() << "Wrong datarate given : " << rate;
}

QString PCanInterface::getBitRate()
{
	for(int i = 0; i < m_baud_list.count(); i++)
	{
		if(m_baud_list.at(i).second == m_conf_bitrate)
		{
			return m_baud_list.at(i).first;
		}
	}

	return "";
}

void PCanInterface::setReadOnly(bool read_only)
{
	m_conf_readOnly = read_only;
}

bool PCanInterface::getReadOnly()
{
	return m_conf_readOnly;
}

void PCanInterface::setChannel(QString channel)
{
	if(m_handle_map.contains(channel))
	{
		m_channel = m_handle_map[channel];
		m_identifier = channel;
	}
}

QString PCanInterface::getChannel()
{
	return m_handle_map.key(m_channel);
}

void PCanInterface::computeRxRate()
{
	static quint32 count = 0;

	if(m_msg_count < count)
		count = 0;

	m_rx_rate = 1000*(m_msg_count - count)/m_timer_rate.interval();

	count = m_msg_count;
}


void PCanInterface::checkForReceivedMessages()
{
	TPCANMsg CANMsg;
	TPCANTimestamp CANTimeStamp;
	TPCANStatus stsResult;

	stsResult = m_pcan->Read(m_channel, &CANMsg, &CANTimeStamp); // Read message from device

	if(stsResult == PCAN_ERROR_QRCVEMPTY)
	{
		// No new messages
		m_timestamp += m_timer_read.interval()/1000.0; // Increase timestamp to approximate real timestamp
	}

	for(int i = 0; i < 100; i++) // Loop reading messages in buffer, read 100 messages max (to make sure not to block the program)
	{

		//qDebug() << "status : " << stsResult;

		if (stsResult != PCAN_ERROR_QRCVEMPTY)
		{
			//qDebug() << "Message received of type " << CANMsg.MSGTYPE;
			if(CANMsg.MSGTYPE == PCAN_MESSAGE_STANDARD) // Not a status message
			{
				//qDebug() << "Good message type";
				// Parse message
				CanTraceMessage msg;
				msg.id = CANMsg.ID;
				msg.type = "Rx";
				msg.rtr = ((CANMsg.MSGTYPE & PCAN_MESSAGE_RTR) != 0); // Request for transmit flag
				msg.number = m_msg_count + 1;

				CANMsg.LEN = ((CANMsg.LEN > 8) ? 8 : CANMsg.LEN); // Make sure there is 8 bytes of data maximum

				for(int i = 0; i < CANMsg.LEN ; i++)
				{
					msg.data.append(CANMsg.DATA[i]);
				}

				// Timestamp
				msg.timestamp = CANTimeStamp.millis / 1000.0 + CANTimeStamp.micros / 1000000.0; // in seconds

				m_timestamp = msg.timestamp; // Update timestamp

				emit newCanMessage(msg);
				m_msg_count++;
				//qDebug() << "msg : " << m_msg_count;
			}
		}
		else if (stsResult == PCAN_ERROR_QRCVEMPTY)
		{
			//qDebug() << "PCanInterface::checkForReceivedMessages empty : " << i;
			break; // Quit loop
		}
		else
		{
			// Error ?
			//qDebug() << "PCAN return code for read message " << stsResult;
			break; // Quit loop
		}

		stsResult = m_pcan->Read(m_channel, &CANMsg, &CANTimeStamp); // Read message from device
	}
}

QString PCanInterface::getBusStatus()
{
	if(m_state != INITIALIZED)
		return "-";

	switch(m_pcan->GetStatus(m_channel))
	{
		case PCAN_ERROR_OK:
			return "OK";

		case PCAN_ERROR_BUSHEAVY:
			return "BUS HEAVY";

		case PCAN_ERROR_BUSLIGHT:
			return "BUS LIGHT";

		case PCAN_ERROR_BUSOFF:
			return "BUS OFF";

		case PCAN_ERROR_BUSPASSIVE:
			return "BUS PASSIVE";

		default:
			return "-";
	}
}

void PCanInterface::writeMessage(CanTraceMessage &msg)
{
	TPCANMsg pcan_msg;
	pcan_msg.ID = msg.id;
	pcan_msg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	pcan_msg.LEN = msg.data.count();


	for(int i = 0; i < msg.data.count(); i++)
	{
		pcan_msg.DATA[i] = msg.data.at(i);
	}

	m_pcan->Write(m_channel, &pcan_msg);

	//qDebug() << "m_pcan->Write returns " << ret;

	// Emit signal to indicate a new message was sent on the bus
	msg.type = "Tx";
	msg.timestamp = m_timestamp;
	emit newCanMessage(msg);
}


//
// Static function
//

QStringList PCanInterface::getAvailableDeviceList()
{
	// List of PCAN device handler (id)
	//typedef QPair<TPCANHandle, QString> HandlePair;
	//QList<HandlePair> handleArray;
	//handleArray[0] = HandlePair(PCAN_USBBUS1, "PCAN_USBBUS1");

	TPCANHandle m_HandlesArray[59];

	m_HandlesArray[0] = PCAN_ISABUS1;
	m_HandlesArray[1] = PCAN_ISABUS2;
	m_HandlesArray[2] = PCAN_ISABUS3;
	m_HandlesArray[3] = PCAN_ISABUS4;
	m_HandlesArray[4] = PCAN_ISABUS5;
	m_HandlesArray[5] = PCAN_ISABUS6;
	m_HandlesArray[6] = PCAN_ISABUS7;
	m_HandlesArray[7] = PCAN_ISABUS8;
	m_HandlesArray[8] = PCAN_DNGBUS1;
	m_HandlesArray[9] = PCAN_PCIBUS1;
	m_HandlesArray[10] = PCAN_PCIBUS2;
	m_HandlesArray[11] = PCAN_PCIBUS3;
	m_HandlesArray[12] = PCAN_PCIBUS4;
	m_HandlesArray[13] = PCAN_PCIBUS5;
	m_HandlesArray[14] = PCAN_PCIBUS6;
	m_HandlesArray[15] = PCAN_PCIBUS7;
	m_HandlesArray[16] = PCAN_PCIBUS8;
	m_HandlesArray[17] = PCAN_PCIBUS9;
	m_HandlesArray[18] = PCAN_PCIBUS10;
	m_HandlesArray[19] = PCAN_PCIBUS11;
	m_HandlesArray[20] = PCAN_PCIBUS12;
	m_HandlesArray[21] = PCAN_PCIBUS13;
	m_HandlesArray[22] = PCAN_PCIBUS14;
	m_HandlesArray[23] = PCAN_PCIBUS15;
	m_HandlesArray[24] = PCAN_PCIBUS16;
	m_HandlesArray[25] = PCAN_USBBUS1;
	m_HandlesArray[26] = PCAN_USBBUS2;
	m_HandlesArray[27] = PCAN_USBBUS3;
	m_HandlesArray[28] = PCAN_USBBUS4;
	m_HandlesArray[29] = PCAN_USBBUS5;
	m_HandlesArray[30] = PCAN_USBBUS6;
	m_HandlesArray[31] = PCAN_USBBUS7;
	m_HandlesArray[32] = PCAN_USBBUS8;
	m_HandlesArray[33] = PCAN_USBBUS9;
	m_HandlesArray[34] = PCAN_USBBUS10;
	m_HandlesArray[35] = PCAN_USBBUS11;
	m_HandlesArray[36] = PCAN_USBBUS12;
	m_HandlesArray[37] = PCAN_USBBUS13;
	m_HandlesArray[38] = PCAN_USBBUS14;
	m_HandlesArray[39] = PCAN_USBBUS15;
	m_HandlesArray[40] = PCAN_USBBUS16;
	m_HandlesArray[41] = PCAN_PCCBUS1;
	m_HandlesArray[42] = PCAN_PCCBUS2;
	m_HandlesArray[43] = PCAN_LANBUS1;
	m_HandlesArray[44] = PCAN_LANBUS2;
	m_HandlesArray[45] = PCAN_LANBUS3;
	m_HandlesArray[46] = PCAN_LANBUS4;
	m_HandlesArray[47] = PCAN_LANBUS5;
	m_HandlesArray[48] = PCAN_LANBUS6;
	m_HandlesArray[49] = PCAN_LANBUS7;
	m_HandlesArray[50] = PCAN_LANBUS8;
	m_HandlesArray[51] = PCAN_LANBUS9;
	m_HandlesArray[52] = PCAN_LANBUS10;
	m_HandlesArray[53] = PCAN_LANBUS11;
	m_HandlesArray[54] = PCAN_LANBUS12;
	m_HandlesArray[55] = PCAN_LANBUS13;
	m_HandlesArray[56] = PCAN_LANBUS14;
	m_HandlesArray[57] = PCAN_LANBUS15;
	m_HandlesArray[58] = PCAN_LANBUS16;

	QString m_HandlesArrayStr[59];

	m_HandlesArrayStr[0] = "PCAN_ISABUS1";
	m_HandlesArrayStr[1] = "PCAN_ISABUS2";
	m_HandlesArrayStr[2] = "PCAN_ISABUS3";
	m_HandlesArrayStr[3] = "PCAN_ISABUS4";
	m_HandlesArrayStr[4] = "PCAN_ISABUS5";
	m_HandlesArrayStr[5] = "PCAN_ISABUS6";
	m_HandlesArrayStr[6] = "PCAN_ISABUS7";
	m_HandlesArrayStr[7] = "PCAN_ISABUS8";
	m_HandlesArrayStr[8] = "PCAN_DNGBUS1";
	m_HandlesArrayStr[9] = "PCAN_PCIBUS1";
	m_HandlesArrayStr[10] = "PCAN_PCIBUS2";
	m_HandlesArrayStr[11] = "PCAN_PCIBUS3";
	m_HandlesArrayStr[12] = "PCAN_PCIBUS4";
	m_HandlesArrayStr[13] = "PCAN_PCIBUS5";
	m_HandlesArrayStr[14] = "PCAN_PCIBUS6";
	m_HandlesArrayStr[15] = "PCAN_PCIBUS7";
	m_HandlesArrayStr[16] = "PCAN_PCIBUS8";
	m_HandlesArrayStr[17] = "PCAN_PCIBUS9";
	m_HandlesArrayStr[18] = "PCAN_PCIBUS10";
	m_HandlesArrayStr[19] = "PCAN_PCIBUS11";
	m_HandlesArrayStr[20] = "PCAN_PCIBUS12";
	m_HandlesArrayStr[21] = "PCAN_PCIBUS13";
	m_HandlesArrayStr[22] = "PCAN_PCIBUS14";
	m_HandlesArrayStr[23] = "PCAN_PCIBUS15";
	m_HandlesArrayStr[24] = "PCAN_PCIBUS16";
	m_HandlesArrayStr[25] = "PCAN_USBBUS1";
	m_HandlesArrayStr[26] = "PCAN_USBBUS2";
	m_HandlesArrayStr[27] = "PCAN_USBBUS3";
	m_HandlesArrayStr[28] = "PCAN_USBBUS4";
	m_HandlesArrayStr[29] = "PCAN_USBBUS5";
	m_HandlesArrayStr[30] = "PCAN_USBBUS6";
	m_HandlesArrayStr[31] = "PCAN_USBBUS7";
	m_HandlesArrayStr[32] = "PCAN_USBBUS8";
	m_HandlesArrayStr[33] = "PCAN_USBBUS9";
	m_HandlesArrayStr[34] = "PCAN_USBBUS10";
	m_HandlesArrayStr[35] = "PCAN_USBBUS11";
	m_HandlesArrayStr[36] = "PCAN_USBBUS12";
	m_HandlesArrayStr[37] = "PCAN_USBBUS13";
	m_HandlesArrayStr[38] = "PCAN_USBBUS14";
	m_HandlesArrayStr[39] = "PCAN_USBBUS15";
	m_HandlesArrayStr[40] = "PCAN_USBBUS16";
	m_HandlesArrayStr[41] = "PCAN_PCCBUS1";
	m_HandlesArrayStr[42] = "PCAN_PCCBUS2";
	m_HandlesArrayStr[43] = "PCAN_LANBUS1";
	m_HandlesArrayStr[44] = "PCAN_LANBUS2";
	m_HandlesArrayStr[45] = "PCAN_LANBUS3";
	m_HandlesArrayStr[46] = "PCAN_LANBUS4";
	m_HandlesArrayStr[47] = "PCAN_LANBUS5";
	m_HandlesArrayStr[48] = "PCAN_LANBUS6";
	m_HandlesArrayStr[49] = "PCAN_LANBUS7";
	m_HandlesArrayStr[50] = "PCAN_LANBUS8";
	m_HandlesArrayStr[51] = "PCAN_LANBUS9";
	m_HandlesArrayStr[52] = "PCAN_LANBUS10";
	m_HandlesArrayStr[53] = "PCAN_LANBUS11";
	m_HandlesArrayStr[54] = "PCAN_LANBUS12";
	m_HandlesArrayStr[55] = "PCAN_LANBUS13";
	m_HandlesArrayStr[56] = "PCAN_LANBUS14";
	m_HandlesArrayStr[57] = "PCAN_LANBUS15";
	m_HandlesArrayStr[58] = "PCAN_LANBUS16";


	PCANDriver pcan;

	// Load library
	pcan.load();

	if(pcan.getStatus() != Ready)
		return QStringList();

	QStringList list;

	int iBuffer;
	TPCANStatus stsResult;
	//bool isFD;

	// TODO : use m_handle_map
	for(int i = 9; i < 59; i++) // Browse table for Plug and Play handles only
	{
		stsResult = pcan.GetValue((TPCANHandle)m_HandlesArray[i], PCAN_CHANNEL_CONDITION, (void*)&iBuffer, sizeof(iBuffer));

		if ((stsResult == PCAN_ERROR_OK) && ((iBuffer & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE))
		{
			stsResult = pcan.GetValue((TPCANHandle)m_HandlesArray[i], PCAN_CHANNEL_FEATURES, (void*)&iBuffer, sizeof(iBuffer));
			//isFD = (stsResult == PCAN_ERROR_OK) && (iBuffer & FEATURE_FD_CAPABLE); // Not used for now
			list.append(m_HandlesArrayStr[i]);
		}
	}


	// Unload
	pcan.unload();

	return list;
}
