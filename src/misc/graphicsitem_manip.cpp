
#include "graphicsitem_manip.h"
#include <QDebug>


QPointF centerHorizontal(QGraphicsItem* item1, QGraphicsItem* item2)
{
	if(item1->parentItem() == 0 && item2->parentItem() == 0)
	{
		// Align two items in the scene

		QPointF c1 = item1->sceneBoundingRect().center(); // Center of item 1
		QPointF c2 = item2->sceneBoundingRect().center(); // Center of item 2
		item1->setX(item1->pos().x() + (c2.x() - c1.x())); // Adjust center of item 1
	}
	else if(item1->parentItem() == item2->parentItem())
	{
		// These are children items from a same parent (Item group)
		// Align according relative to parent coordinates

		QPointF c1 = item1->mapRectToParent(item1->boundingRect()).center(); // Center of item 1
		QPointF c2 = item2->mapRectToParent(item2->boundingRect()).center(); // Center of item 2
		item1->setX(item1->pos().x() + (c2.x() - c1.x()));
	}

	return item1->pos();
}


QPointF centerVertical(QGraphicsItem* item1, QGraphicsItem* item2)
{
	if(item1->parentItem() == 0 && item2->parentItem() == 0)
	{
		// Align two items in the scene

		QPointF c1 = item1->sceneBoundingRect().center(); // Center of item 1
		QPointF c2 = item2->sceneBoundingRect().center(); // Center of item 2
		item1->setY(item1->pos().y() + (c2.y() - c1.y())); // Adjust center of item 1
	}
	else if(item1->parentItem() == item2->parentItem())
	{
		// These are children items from a same parent (Item group)
		// Align according relative to parent coordinates

		QPointF c1 = item1->mapRectToParent(item1->boundingRect()).center(); // Center of item 1
		QPointF c2 = item2->mapRectToParent(item2->boundingRect()).center(); // Center of item 2
		item1->setY(item1->pos().y() + (c2.y() - c1.y())); // Adjust center of item 1
	}

	return item1->pos();
}

void alignLeft(QList<QGraphicsItem*> items)
{
	qreal xmin;

	// Find the leftmost item
	for(int i = 0; i < items.count(); i++)
	{
		if(i == 0)
			xmin = items.at(i)->sceneBoundingRect().left();
		else
			xmin = (items.at(i)->sceneBoundingRect().left() < xmin) ? items.at(i)->sceneBoundingRect().left() : xmin; // Store lowest
	}

	// Align all items to the left
	for(int i = 0; i < items.count(); i++)
	{
		QGraphicsItem* item = items.at(i);
		qreal x = item->sceneBoundingRect().left();

		item->setX(item->x() + (xmin - x));
	}
}


void alignRight(QList<QGraphicsItem*> items)
{
	qreal xmax;

	// Find the leftmost item
	for(int i = 0; i < items.count(); i++)
	{
		if(i == 0)
			xmax = items.at(i)->sceneBoundingRect().right();
		else
			xmax = (items.at(i)->sceneBoundingRect().right() > xmax) ? items.at(i)->sceneBoundingRect().right() : xmax; // Store highest
	}

	// Align all items to the right
	for(int i = 0; i < items.count(); i++)
	{
		QGraphicsItem* item = items.at(i);
		qreal x = item->sceneBoundingRect().right();

		item->setX(item->x() + (xmax - x));
	}
}


void alignTop(QList<QGraphicsItem*> items)
{
	qreal ymin;

	// Find the leftmost item
	for(int i = 0; i < items.count(); i++)
	{
		if(i == 0)
			ymin = items.at(i)->sceneBoundingRect().top();
		else
			ymin = (items.at(i)->sceneBoundingRect().top() < ymin) ? items.at(i)->sceneBoundingRect().top() : ymin; // Store highest
	}

	// Align all items to the top
	for(int i = 0; i < items.count(); i++)
	{
		QGraphicsItem* item = items.at(i);
		qreal y = item->sceneBoundingRect().top();

		item->setY(item->y() + (ymin - y));
	}
}

void alignBottom(QList<QGraphicsItem*> items)
{
	qreal ymax;

	// Find the bottom most item
	for(int i = 0; i < items.count(); i++)
	{		
		if(i == 0)
			ymax = items.at(i)->sceneBoundingRect().bottom();
		else
			ymax = (items.at(i)->sceneBoundingRect().bottom() > ymax) ? items.at(i)->sceneBoundingRect().bottom() : ymax; // Store highest
	}

	// Align all items to the bottom
	for(int i = 0; i < items.count(); i++)
	{
		QGraphicsItem* item = items.at(i);
		qreal y = item->sceneBoundingRect().bottom();

		item->setY(item->y() + (ymax - y));
	}
}


void alignCenterV(QList<QGraphicsItem*> items)
{
	qreal xref;
	for(int i = 0; i < items.count(); i++)
	{
		if(i == 0)
			xref = items.at(0)->sceneBoundingRect().center().x();
		else
		{
			QGraphicsItem* item = items.at(i);
			qreal x = item->sceneBoundingRect().center().x();

			item->setX(item->x() + (xref - x));
		}
	}
}

void alignCenterH(QList<QGraphicsItem*> items)
{
	qreal yref;
	for(int i = 0; i < items.count(); i++)
	{
		if(i == 0)
			yref = items.at(0)->sceneBoundingRect().center().y();
		else
		{
			QGraphicsItem* item = items.at(i);
			qreal y = item->sceneBoundingRect().center().y();

			item->setY(item->y() + (yref - y));
		}
	}
}


void distributeH(QList<QGraphicsItem*> items)
{
	QList<QGraphicsItem*> ordered_list; // Ordered according to y value

	if(items.count() < 2)
		return;

	for(int i = 0; i < items.count(); i++)
	{
		if(ordered_list.count() == 0)
		{
			ordered_list.append(items.at(i));
			continue;
		}

		// Build an ordered list
		for(int j = 0; j < ordered_list.count(); j++)
		{

			if(items.at(i)->sceneBoundingRect().center().x() < ordered_list.at(j)->sceneBoundingRect().center().x())
			{
				ordered_list.insert(j, items.at(i));
				break;
			}

			if(j == ordered_list.count() - 1)
			{
				ordered_list.append(items.at(i));
				break;
			}
		}
	}

	// Adjust the items position (distribute equally over y)
	qreal xmin = ordered_list.first()->sceneBoundingRect().center().x();
	qreal space = ordered_list.last()->sceneBoundingRect().center().x() - xmin;
	qreal distance = space/(ordered_list.count() - 1); // space that will seperate each item's center

	for(int j = 1; j < ordered_list.count() - 1; j++)
	{
		QGraphicsItem* item = ordered_list.at(j);
		qreal x = item->sceneBoundingRect().center().x();
		qreal xref = xmin + j*distance;

		item->setX(item->x() + (xref - x));
	}
}


void distributeV(QList<QGraphicsItem*> items)
{
	QList<QGraphicsItem*> ordered_list; // Ordered according to y value

	if(items.count() < 2)
		return;

	for(int i = 0; i < items.count(); i++)
	{
		if(ordered_list.count() == 0)
		{
			ordered_list.append(items.at(i));
			continue;
		}

		// Build an ordered list
		for(int j = 0; j < ordered_list.count(); j++)
		{
			if(items.at(i)->sceneBoundingRect().center().y() < ordered_list.at(j)->sceneBoundingRect().center().y())
			{
				ordered_list.insert(j, items.at(i));
				break;
			}

			if(j == ordered_list.count() - 1)
			{
				ordered_list.append(items.at(i));
				break;
			}
		}
	}

	// Adjust the items position (distribute equally over y)
	qreal ymin = ordered_list.first()->sceneBoundingRect().center().y();
	qreal space = ordered_list.last()->sceneBoundingRect().center().y() - ymin;
	qreal distance = space/(ordered_list.count() - 1); // space that will seperate each item's center

	for(int j = 1; j < ordered_list.count() - 1; j++)
	{
		QGraphicsItem* item = ordered_list.at(j);
		qreal y = item->sceneBoundingRect().center().y();
		qreal yref = ymin + j*distance;

		item->setY(item->y() + (yref - y));
	}
}
