#ifndef GRAPHICSITEM_MANIP_H
#define GRAPHICSITEM_MANIP_H

#include <QGraphicsItem>

// Center item1 horizontally with item2 changing item1's position
// Returns the position applied on item1
QPointF centerHorizontal(QGraphicsItem* item1, QGraphicsItem* item2);

// Center item 1 vertically with item 2
// Returns the position applied on item1
QPointF centerVertical(QGraphicsItem* item1, QGraphicsItem* item2);

// Align all items to leftmost item
void alignLeft(QList<QGraphicsItem*> items);

// Align all items to rightmost item
void alignRight(QList<QGraphicsItem*> items);

// Align all items to topmost item
void alignTop(QList<QGraphicsItem*> items);

// Align all items to bottommost item
void alignBottom(QList<QGraphicsItem*> items);

// Align all items center verticaly to the first item of the list
void alignCenterV(QList<QGraphicsItem*> items);

// Align all items center horizontally to the firts item of list
void alignCenterH(QList<QGraphicsItem*> items);

// Distribute items horizontally
void distributeH(QList<QGraphicsItem *> items);

// Distribute item vertically
void distributeV(QList<QGraphicsItem*> items);

#endif // GRAPHICSITEM_MANIP_H
