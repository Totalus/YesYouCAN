#include "baritem.h"

BarItem::BarItem(GraphicsObjectParameters_t params, QGraphicsItem *parent) : CustomGraphicsItem(parent)
{
	params.itemType = CustomGraphicsItem::BAR_ITEM;
	m_params = params;

	// Attributes
	m_sigValue.name = params.signalName;
	m_sigValue.unit = params.signalUnits;

	createGraphicsItems();

	resetSignalValue();
}

BarItem::BarItem(QDataStream &stream, quint8 version) : CustomGraphicsItem()
{
	createGraphicsItems(); // Create graphics item before deserialization so position is uptaded properly
	deserialize(stream, version); // Recover attributes from stream

	updateGraphicsItems();

	m_sigValue.name = m_params.signalName;
	m_sigValue.unit = m_params.signalUnits;
	resetSignalValue();
}

void BarItem::updateGraphicsItems()
{
	m_identifierText->setFont(m_params.font);
	m_valueText->setFont(m_params.font);
	m_valueRect->setBrush(QBrush(m_params.color, Qt::SolidPattern));
	m_valueRect->setPen(QPen(Qt::NoPen));

	QRectF r = m_valueRect->rect();
	r.setWidth(m_params.width);
	m_valueRect->setRect(r);

	QColor lightgray(Qt::lightGray);
	lightgray.setAlpha(120);
	m_outlineRect->setPen(lightgray);

	r = m_outlineRect->rect();
	r.setHeight(m_params.height);
	r.setWidth(m_params.width);
	m_outlineRect->setRect(r);

	if(m_params.showAlias && m_params.showSignalName)
		m_identifierText->setText(m_params.signalAlias + " (" + m_params.signalName + ")");
	else if(m_params.showAlias)
		m_identifierText->setText(m_params.signalAlias);
	else if(m_params.showSignalName)
		m_identifierText->setText(m_params.signalName);
	else
		m_identifierText->setText("");

	// Positioning

	if(m_params.orientation == 0) // Vertical
	{
		m_valueRect->setRotation(180);
		m_outlineRect->setRotation(180);

		centerHorizontal(m_identifierText, m_valueRect);
		m_identifierText->setY(5);
	}
	else // Horizontal
	{
		m_valueRect->setRotation(270);
		m_outlineRect->setRotation(270);
		centerVertical(m_identifierText, m_valueRect);
		m_identifierText->setX(-m_identifierText->boundingRect().width() - 10); // move to left of the rectangle
		centerVertical(m_valueText, m_valueRect);
	}

	// To update bounding rect, remove and re-add to group (Qt does not update the boundingRect otherwise)
	// Related Qt bug : QTBUG-25974
	// TODO : enlever une fois le bug résolu
	removeFromGroup(m_identifierText);
	addToGroup(m_identifierText);
}

void BarItem::setParameters(GraphicsObjectParameters_t params)
{
	CustomGraphicsItem::setParameters(params);

	updateGraphicsItems();
}

void BarItem::createGraphicsItems()
{
	// Graphics items
	m_identifierText = new QGraphicsSimpleTextItem("123"); // Set text to ensure a selection rectangle around the object
	m_valueText = new QGraphicsSimpleTextItem("val");
	m_valueRect = new QGraphicsRectItem(0, 0, m_params.width, m_params.height, this);
	m_outlineRect = new QGraphicsRectItem(0,0, m_params.width, m_params.height, this);

	addToGroup(m_identifierText);
	addToGroup(m_valueText);
	addToGroup(m_valueRect);
	addToGroup(m_outlineRect);

	updateGraphicsItems();
}

BarItem::~BarItem()
{
	if(m_identifierText != 0)
		delete m_identifierText;

	if(m_outlineRect != 0)
		delete m_outlineRect;

	if(m_valueRect != 0)
		delete m_valueRect;

	if(m_valueText != 0)
		delete m_valueText;
}

void BarItem::redraw()
{
	//	Redraw the item depending on configuration and signal value

	//
	// Rectangle bar
	//

	bool ok;
	double sigVal = m_sigValue.valueStr.toDouble(&ok);

	if(m_sigValue.max > m_sigValue.min) // Valid max and min values
	{
		if(ok)
		{
			// Update height
			QRectF r = m_valueRect->rect();

			qreal newHeight;
			if(m_sigValue.max < sigVal)
				newHeight = m_params.height;
			else if(m_sigValue.min > sigVal)
				newHeight = 0;
			else
				newHeight = (sigVal - m_sigValue.min)/(m_sigValue.max - m_sigValue.min)*m_params.height;

			r.setHeight(newHeight);
			m_valueRect->setRect(r);
		}
		else
		{
			// Error : bad signal value
			QRectF r = m_valueRect->rect();
			r.setHeight(0);
			m_valueRect->setRect(r);
		}
	}
	else
	{
		QRectF r = m_valueRect->rect();

		qreal newHeight;
		if(m_sigValue.max < sigVal)
			newHeight = m_params.height;
		else if(m_sigValue.min > sigVal)
			newHeight = 0;
		else
			newHeight = sigVal/100.0*m_params.height;;

		r.setHeight(newHeight);
		m_valueRect->setRect(r);
	}

	//
	// Value Text (represents the signal value)
	//

	QString line = "";

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

	if(ok)
	{
		if(sigVal > m_sigValue.max)
		line += " > max";
		else if(sigVal < m_sigValue.min)
		line += " < min";
	}

	m_valueText->setText(line);

	// Color value text red if > max or < min
	QBrush brush = m_valueText->brush();
	if(ok && (sigVal > m_sigValue.max || sigVal < m_sigValue.min))
		brush.setColor(QColor(Qt::red));
	else
		brush.setColor(QColor(Qt::black));
	m_valueText->setBrush(brush);


	// Adjust positioning
	if(m_params.orientation == 0) // Vertical
	{
		QPointF p = centerHorizontal(m_valueText, m_valueRect); // Center horizontally because width of text changes
		p.setX(p.x() + 2); // Makes a overall better centering with the outline rect
		if(sigVal < m_sigValue.min)
			p.setY(0 - 5 - m_valueText->boundingRect().height());
		else
			p.setY(-m_valueRect->boundingRect().height() - 5 - m_valueText->boundingRect().height());
		m_valueText->setPos(p);
	}
	else // Horizontal
	{
		QPointF p = m_valueText->pos(); // No vertical centering here because height of text should remain the same
		if(sigVal < m_sigValue.min)
			p.setX(m_valueText->boundingRect().height()); // Use height() because of rotation
		else
			p.setX(m_valueRect->boundingRect().height() + m_valueText->boundingRect().height()); // Use height() because of rotation

		m_valueText->setPos(p);
	}
}

void BarItem::updateSignalValue(SignalValue_t newValue)
{
	if(newValue.name != m_params.signalName)
		return;

	m_sigValue = newValue;
	redraw();
}

void BarItem::resetSignalValue()
{
	m_sigValue.rawValue = 0;
	m_sigValue.valueStr = "?";
	redraw();
}

QString BarItem::getSignalName()
{
	return m_sigValue.name;
}
