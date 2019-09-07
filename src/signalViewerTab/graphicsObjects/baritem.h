#ifndef BARITEM_H
#define BARITEM_H


#include "customgraphicsitem.h"
#include "graphicsitem_manip.h"
#include <QBrush>
#include <QPen>


class BarItem : public CustomGraphicsItem
{
	Q_OBJECT

	public:
		//BarItem(QString signalName, QString signalUnits, QGraphicsItem *parent = 0);
		BarItem(GraphicsObjectParameters_t params, QGraphicsItem *parent = 0);
		BarItem(QDataStream& stream, quint8 version = 0); // Create an instance and populate reading data from the stream
		~BarItem();

		virtual void redraw() Q_DECL_OVERRIDE;
		virtual QString getSignalName() Q_DECL_OVERRIDE;
		virtual void setParameters(GraphicsObjectParameters_t params);

	public slots:
		virtual void updateSignalValue(SignalValue_t newValue);
		virtual void resetSignalValue();

	private:
		// Sub item elements
		QGraphicsSimpleTextItem *m_identifierText;	// Signal name or alias
		QGraphicsSimpleTextItem *m_valueText;		// Signal value or raw value
		QGraphicsRectItem *m_valueRect;				// Rectangle for value
		QGraphicsRectItem *m_outlineRect;			// Outline

		// Attributes
		SignalValue_t m_sigValue;

		void createGraphicsItems();
		void updateGraphicsItems();
};

#endif // BARITEM_H
