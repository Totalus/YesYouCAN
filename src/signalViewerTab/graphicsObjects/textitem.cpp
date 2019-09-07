#include "textitem.h"

/*
TextItem::TextItem(QString signalName, QString signalUnits, QGraphicsItem *parent) : CustomGraphicsItem(parent)
{
	m_params.itemType = CustomGraphicsItem::TEXT_ITEM;
	m_params.signalName = signalName;
	m_params.signalUnits = signalUnits;

	// Attributes
	m_sigValue.name = signalName;
	m_sigValue.unit = signalUnits;

	createGraphicsItems();
	resetSignalValue();
}
*/

TextItem::TextItem(GraphicsObjectParameters_t params, QGraphicsItem *parent) : CustomGraphicsItem(parent)
{
	params.itemType = CustomGraphicsItem::TEXT_ITEM;
	m_params = params;

	// Attributes
	m_sigValue.name = params.signalName;
	m_sigValue.unit = params.signalUnits;

	createGraphicsItems();

	resetSignalValue();
}

TextItem::TextItem(QDataStream &stream, quint8 version) : CustomGraphicsItem()
{
	createGraphicsItems(); // Create graphics item before deserialization so position is uptaded properly
	deserialize(stream, version); // Recover attributes from stream

	updateGraphicsItems();

	m_sigValue.name = m_params.signalName;
	m_sigValue.unit = m_params.signalUnits;
	resetSignalValue();
}

void TextItem::updateGraphicsItems()
{
	m_identifierText->setFont(m_params.font);
	//setRotation(m_params.angle); // Pas pertinent pour du texte...
}

void TextItem::setParameters(GraphicsObjectParameters_t params)
{
	CustomGraphicsItem::setParameters(params);
	updateGraphicsItems();
}

void TextItem::createGraphicsItems()
{
	// Graphics items

	m_identifierText = new QGraphicsSimpleTextItem(m_params.signalName.isEmpty() ? "123" : m_params.signalName); // Set text to ensure a selection rectangle around the object
	addToGroup(m_identifierText);
	m_identifierText->setText("");

	updateGraphicsItems();
}

TextItem::~TextItem()
{
	if(m_identifierText != 0)
		delete m_identifierText;
}

void TextItem::redraw()
{
	//
	//	Redraw the item depending on configuration
	//

	QString line = "";

	if(m_params.showAlias && m_params.showSignalName)
		line += m_params.signalAlias + " (" + m_params.signalName + ")";
	else if(m_params.showAlias)
		line += m_params.signalAlias;
	else if(m_params.showSignalName)
		line += m_params.signalName;

	if(m_params.showPhysicalValue)
	{
		if(!line.isEmpty())
			line += " : ";

		line += m_sigValue.valueStr;

		if(m_params.overrideUnits)
			line += " " + m_params.signalUnits;
		else
			line += " " + m_sigValue.unit;
	}

	if(m_params.base != 0)
	{
		if(m_params.showPhysicalValue)
			line += " (";
		else
			line += " : ";

		switch(m_params.base)
		{
			case 2:
				line += "0b" + QString::number(m_sigValue.rawValue, 2);
				break;

			case 10:
				line += QString::number(m_sigValue.rawValue, 10);
				break;

			default:
				line += "0x" + QString::number(m_sigValue.rawValue, 16);
				break;
		}

		if(m_params.showPhysicalValue)
			line += ")";
	}


	/*
	if(m_params.showPercent)
	{
		if(m_params.showPhysicalValue || m_params.base != 0)
			line += " (";
		else
			line += " : ";

		// TODO

		if(m_params.showPhysicalValue || m_params.base != 0)
			line += ")";
	}
	*/

	m_identifierText->setText(line);
	//qDebug() << "redraw()";
}

void TextItem::updateSignalValue(SignalValue_t newValue)
{
	if(newValue.name != m_params.signalName)
		return;

	m_sigValue = newValue;
	redraw();
}

void TextItem::resetSignalValue()
{
	m_sigValue.rawValue = 0;
	m_sigValue.valueStr = "-";
	redraw();
}

QString TextItem::getSignalName()
{
	return m_sigValue.name;
}
