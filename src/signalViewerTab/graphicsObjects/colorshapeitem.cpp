#include "colorshapeitem.h"

ColorShapeItem::ColorShapeItem(GraphicsObjectParameters_t params, QGraphicsItem *parent) : CustomGraphicsItem(parent)
{
	params.itemType = CustomGraphicsItem::COLOR_SHAPE_ITEM;
	m_params = params;

	// Attributes
	m_sigValue.name = params.signalName;
	m_sigValue.unit = params.signalUnits;

	createGraphicsItems();

	resetSignalValue();
}

ColorShapeItem::ColorShapeItem(QDataStream &stream, quint8 version) : CustomGraphicsItem()
{
	createGraphicsItems(); // Create graphics item before deserialization so position is uptaded properly
	deserialize(stream, version); // Recover attributes from stream

	updateGraphicsItems();

	m_sigValue.name = m_params.signalName;
	m_sigValue.unit = m_params.signalUnits;
	resetSignalValue();
}

void ColorShapeItem::updateGraphicsItems()
{
	m_identifierText->setFont(m_params.font);
	m_valueText->setFont(m_params.font);

	m_ellipse->setRect(0,0, m_params.width, m_params.width);
	m_ellipse->setBrush(QBrush(m_params.minColor, Qt::SolidPattern));
	m_ellipse->setPen(QPen(Qt::NoPen));

	alignCenterV(QList<QGraphicsItem*>() << m_ellipse << m_identifierText);
	m_identifierText->setY(3 + m_ellipse->pos().y() + m_ellipse->boundingRect().height());
}

void ColorShapeItem::setParameters(GraphicsObjectParameters_t params)
{
	CustomGraphicsItem::setParameters(params);
	updateGraphicsItems();
}

void ColorShapeItem::createGraphicsItems()
{
	// Graphics items
	m_ellipse = new QGraphicsEllipseItem(0,0,m_params.width,m_params.width);
	addToGroup(m_ellipse);

	m_identifierText = new QGraphicsSimpleTextItem(m_params.signalName.isEmpty() ? "123" : m_params.signalName); // Set text to ensure a selection rectangle around the object
	addToGroup(m_identifierText);
	m_identifierText->setText("");

	m_valueText = new QGraphicsSimpleTextItem("123");
	addToGroup(m_valueText);
	m_valueText->setText("");

	updateGraphicsItems();
}

ColorShapeItem::~ColorShapeItem()
{
	if(m_identifierText != 0)
		delete m_identifierText;

	if(m_valueText != 0)
		delete m_valueText;

	if(m_ellipse != 0)
		delete m_ellipse;
}

void ColorShapeItem::redraw()
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

	m_identifierText->setText(line);

	line = "";

	if(m_params.showPhysicalValue)
	{
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

	m_valueText->setText(line);

	// Gradient color
	if(m_sigValue.max > m_sigValue.min)
	{
		QBrush brush = m_ellipse->brush();

		if(m_sigValue.value > m_sigValue.max)
		{
			brush.setColor(m_params.maxColor);
		}
		else if(m_sigValue.value < m_sigValue.min)
		{
			brush.setColor(m_params.minColor);
		}
		else
		{
			double ratio = (m_sigValue.value - m_sigValue.min)/(m_sigValue.max - m_sigValue.min);

			int h = (m_params.maxColor.hsvHue()*ratio + m_params.minColor.hsvSaturation()*(1-ratio));
			int s = (m_params.maxColor.hsvSaturation()*ratio + m_params.minColor.hsvSaturation()*(1-ratio));
			int v = (m_params.maxColor.value()*ratio + m_params.minColor.value()*(1-ratio));

			int r = (m_params.maxColor.red()*ratio + m_params.minColor.red()*(1-ratio));
			int g = (m_params.maxColor.green()*ratio + m_params.minColor.green()*(1-ratio));
			int b = (m_params.maxColor.blue()*ratio + m_params.minColor.blue()*(1-ratio));

			qDebug() << "rgb" << r << g << b << QColor::fromRgb(r,g,b);
			qDebug() << "hsv" << h << s << v << QColor::fromHsv(h,s,v);

			//brush.setColor(QColor::fromHsv(h,s,v));
			brush.setColor(QColor::fromRgb(r,g,b));
		}

		//brush.setStyle(Qt::SolidPattern);
		m_ellipse->setBrush(brush);
	}

	alignCenterH(QList<QGraphicsItem*>() << m_ellipse << m_valueText);
	alignCenterV(QList<QGraphicsItem*>() << m_ellipse << m_valueText << m_identifierText);
}

void ColorShapeItem::updateSignalValue(SignalValue_t newValue)
{
	if(newValue.name != m_params.signalName)
		return;

	m_sigValue = newValue;
	redraw();
}

void ColorShapeItem::resetSignalValue()
{
	m_sigValue.rawValue = 0;
	m_sigValue.valueStr = "-";
	redraw();
}

QString ColorShapeItem::getSignalName()
{
	return m_sigValue.name;
}
