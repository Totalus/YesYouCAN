#include "customgraphicsitem.h"

CustomGraphicsItem::CustomGraphicsItem(QGraphicsItem *parent) : QGraphicsItemGroup(parent)
{
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

	// Default values
	m_params.itemType = CustomGraphicsItem::INVALID;
	m_params.showSignalName = false;
	m_params.showAlias = false;
	m_params.showPhysicalValue = true;
	m_params.showPercent = false;
	m_params.overrideUnits = false;
	m_params.angle = 0;
	m_params.orientation = 0;
	m_params.min = 0;
	m_params.max = 0;
	m_params.minColor = QColor();
	m_params.color = QColor();
	m_params.maxColor = QColor();
	m_params.gradient = false;
	m_params.font = QFont();
	m_params.shape = QString();
	m_params.size = 1;
	m_params.height = 1;
	m_params.width = 1;
	m_params.base = 0;
}


GraphicsObjectParameters_t CustomGraphicsItem::getParameters()
{
	return m_params;
}

void CustomGraphicsItem::setParameters(GraphicsObjectParameters_t params)
{
	m_params = params;
}

void CustomGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsItemGroup::contextMenuEvent(event);

	if(!event->isAccepted())
	{
		emit itemRightClicked(this);
		event->accept(); // The scene accepts the event
	}
}

void CustomGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItemGroup::mouseDoubleClickEvent(event);

	emit itemDoubleClicked(this);
}

/*
QRectF CustomGraphicsItem::boundingRect() const
{
	qDebug() << "boudingRect()" << childrenBoundingRect();
	return childrenBoundingRect();
}
//*/

//
// Data stream functions (for serialization)
//

QDataStream& CustomGraphicsItem::serialize(QDataStream &out)
{
	//qDebug() << "serialize(CustomGraphicsItem)";

	// Order here is very important, if changed, it will affect the file format

	// Save class attributes
	out << m_params.itemType; // itemType serialized twice for deserialization process
	out << m_params.itemType;
	out << m_params.signalName;
	out << m_params.showSignalName;
	out << m_params.signalAlias;
	out << m_params.showAlias;
	out << m_params.showPhysicalValue;
	out << m_params.showPercent;
	out << m_params.overrideUnits;
	out << m_params.signalUnits;
	out << m_params.angle;
	out << m_params.orientation;
	out << m_params.min;
	out << m_params.max;
	out << m_params.minColor;
	out << m_params.color;
	out << m_params.maxColor;
	out << m_params.gradient;
	out << m_params.font;
	out << m_params.shape;
	out << m_params.misc;
	out << m_params.size;
	out << m_params.width;
	out << m_params.height;
	out << m_params.base;



	out << this->pos(); // Item position

	//qDebug() << "Position " << pos();

	return out;
}

QDataStream& CustomGraphicsItem::deserialize(QDataStream &in, quint8 version)
{
	// Order here is very important, if changed, it will affect the file format

	in >> m_params.itemType; // Read once before deserialization
	in >> m_params.signalName;
	in >> m_params.showSignalName;
	in >> m_params.signalAlias;
	in >> m_params.showAlias;
	in >> m_params.showPhysicalValue;
	in >> m_params.showPercent;
	in >> m_params.overrideUnits;
	in >> m_params.signalUnits;
	in >> m_params.angle;
	in >> m_params.orientation;
	in >> m_params.min;
	in >> m_params.max;
	in >> m_params.minColor;
	in >> m_params.color;
	in >> m_params.maxColor;
	in >> m_params.gradient;
	in >> m_params.font;
	in >> m_params.shape;
	in >> m_params.misc;
	in >> m_params.size;
	in >> m_params.width;
	in >> m_params.height;
	in >> m_params.base;

	// Restore class attributes
	QPointF p;
	in >> p;	setPos(p); // Restore GroupItem position

	return in;
}

