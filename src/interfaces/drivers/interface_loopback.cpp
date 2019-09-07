#include "interface_loopback.h"

LoopbackInterface::LoopbackInterface(QObject *parent, QString identifier) : HwInterface(parent, identifier)
{
	m_msg_count = 0;
}

bool LoopbackInterface::initialize()
{
	return true;
}

void LoopbackInterface::deinitialize()
{
}

bool LoopbackInterface::setCapture(bool enable)
{
	// TODO : store in variable and check before sending signal in writeMessage ?
	return true;
}

bool LoopbackInterface::isCapturing()
{
	return true;
}

InterfaceType_t LoopbackInterface::interfaceType()
{
	return INTERFACE_LOOPBACK;
}

QString LoopbackInterface::interfaceTypeStr()
{
	return QString("Loopback");
}

QString LoopbackInterface::getDescription()
{
	return QString("Loopback");
}

quint32 LoopbackInterface::getMessageCount()
{
	return m_msg_count;
}

QString LoopbackInterface::getErrorStr()
{
	return QString();
}

void LoopbackInterface::writeMessage(CanTraceMessage &msg)
{
	msg.type = "Rx";
	msg.timestamp = QTime::currentTime().msecsSinceStartOfDay()/1000.0; // Set timestamp from local clock
	msg.number = ++m_msg_count;
	emit newCanMessage(msg); // Loopback, message written to this interface are received by this interface
}
