#include "interface_hwinterface.h"
#include "document.h"

HwInterface::HwInterface(QObject *parent, QString identifier) : QObject(parent)
{
	m_rx_rate = 0;
	m_identifier = identifier;
	m_active = false;
	m_state = OFFLINE;

	// Document
	m_document = new Document(CAN_INTERFACE);
	m_document->setDocumentName("Hardware interface");
	m_document->setHwInterface(this);
}

bool HwInterface::isAvailable()
{
	return true;
}

int HwInterface::getRxRate()
{
	return m_rx_rate;
}

bool HwInterface::isActive()
{
	return m_active;
}

QString HwInterface::getIdentifier()
{
	return m_identifier;
}

void HwInterface::setIdentifier(QString id)
{
	m_identifier = id;
}

QString HwInterface::getName()
{
	return m_name;
}

void HwInterface::setName(QString name)
{
	m_name = name;
}

InterfaceState_t HwInterface::getState()
{
	return m_state;
}

Document* HwInterface::getDocument()
{
	return m_document;
}

void HwInterface::writeMessage(CanTraceMessage &msg)
{
	// Do nothing (not supported)
	return;
}

QStringList HwInterface::getCapabilities()
{
	return m_capabilities;
}
