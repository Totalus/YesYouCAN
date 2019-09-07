#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "customgraphicsitem.h"


class TextItem : public CustomGraphicsItem
{
	Q_OBJECT

	public:
		//TextItem(QString signalName, QString signalUnits, QGraphicsItem *parent = 0);
		TextItem(GraphicsObjectParameters_t params, QGraphicsItem *parent = 0);
		TextItem(QDataStream& stream, quint8 version = 0); // Create an instance and populate reading data from the stream
		~TextItem();

		virtual void redraw() Q_DECL_OVERRIDE;
		virtual QString getSignalName() Q_DECL_OVERRIDE;
		virtual void setParameters(GraphicsObjectParameters_t params);

	public slots:
		virtual void updateSignalValue(SignalValue_t newValue);
		virtual void resetSignalValue();

	private:
		// Sub item elements
		QGraphicsSimpleTextItem *m_identifierText;	// Signal name or alias
		//QGraphicsSimpleTextItem *m_valueText;		// Signal value or raw value

		// Attributes
		SignalValue_t m_sigValue;

		void createGraphicsItems();
		void updateGraphicsItems();
};

#endif // TEXTITEM_H
