#ifndef CUSTOMGRAPHICSITEM_H
#define CUSTOMGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItemGroup>
#include <QFont>
#include <QtWidgets/QGraphicsSceneContextMenuEvent>


#include "struct_cansignal.h"


//
//	Parameter structure for graphics items/objects (for Watcher window)
//

typedef struct
{
	// Used
	qint32 itemType;			// TEXT_ITEM, BAR, COLOR
	QString signalName;
	bool showSignalName;
	QString signalAlias;
	bool showAlias;
	bool showPhysicalValue;

	// Reserved
	bool showPercent;		// Percentage value (when using min and max)
	bool overrideUnits;		// Use units from signalUnits (below), otherwise use signal unit from DBC file
	QString signalUnits;	// Used when overrideUnits is true
	qint32 angle;			// Object Angle
	qint32 orientation;		// Vertical, Horizontal, ...
	qint32 size;			// Size of a circle, bar or other
	qint32 height;
	qint32 width;
	double min;				// Minimum value
	double max;				// Maximum value
	QColor minColor;		// Color when below minimum
	QColor color;			// Color when between min and max or default color
	QColor maxColor;		// Color when over maximum
	bool gradient;			// Gradient color between min and max
	QFont font;				// Text font
	QString shape;			// Item shape (square, round, ...)
	QString misc;			// Reserved
	qint8 base;				// Base for displayint raw value (10 = decimal, 16 = hexadecimal, 2 = binary, 0 = hide)
} GraphicsObjectParameters_t;



//
// Provides a graphical item/object
//

class CustomGraphicsItem : public QObject, public QGraphicsItemGroup
{
	Q_OBJECT

	public:

		enum // Item subtype
		{
			TEXT_ITEM = 0,			// Text + signal value
			BAR_ITEM = 1,			// Bar
			COLOR_SHAPE_ITEM = 2,	// Colored shape
			INVALID = 100
		};

		CustomGraphicsItem(QGraphicsItem *parent = 0);

		virtual QString getSignalName() = 0;
		virtual void redraw() = 0;

		virtual GraphicsObjectParameters_t getParameters();
		virtual void setParameters(GraphicsObjectParameters_t m_params);

		//virtual QRectF boundingRect() const Q_DECL_OVERRIDE;


		// Serialization functions (for storing item in binary file)
		virtual QDataStream& serialize(QDataStream &);
		virtual QDataStream& deserialize(QDataStream &, quint8 version = 0);

	signals:
		void itemRightClicked(CustomGraphicsItem* item);
		void itemDoubleClicked(CustomGraphicsItem* item);

	public slots:
		virtual void updateSignalValue(SignalValue_t newValue) = 0;
		virtual void resetSignalValue() = 0;

		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	protected:
		GraphicsObjectParameters_t m_params;
};

#endif // CUSTOMGRAPHICSITEM_H
