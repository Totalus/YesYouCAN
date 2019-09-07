#include "customgraphicsscene.h"

CustomGraphicsScene::CustomGraphicsScene(QMenu *contextMenu, QWidget *parent) : QGraphicsScene(parent)
{
	m_contextMenu = contextMenu;
}


void CustomGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsScene::contextMenuEvent(event); // Send event to items

	if(!event->isAccepted()) // No items accepted the event
	{
		/*qDebug() << (itemAt(event->scenePos(), QTransform()) == 0);
		qDebug() << (itemAt(event->scenePos(), QTransform()) == 0);
		CustomGraphicsItem *item = qgraphicsitem_cast<CustomGraphicsItem*>(itemAt(event->scenePos(), QTransform()));
		qDebug() << (item == 0);
		if(itemAt(event->scenePos(), QTransform()) != 0)
			qDebug() << (itemAt(event->scenePos(), QTransform())->type());

		if(item != 0)
		{
			qDebug() << item->flags();
			// Specific menu when item is under cursor (delete, edit, ...)
		}
		else
		{
		}*/

		m_contextMenu->exec(event->screenPos());

		event->accept(); // The scene accepts the event
	}
}


void CustomGraphicsScene::addItem(CustomGraphicsItem *item)
{
	//qDebug() << "Graphics item added to list";
	m_list.append(item);
	QGraphicsScene::addItem(item); // Add item to scene
}

QList<CustomGraphicsItem*> CustomGraphicsScene::itemList()
{
	return m_list;
}

void CustomGraphicsScene::resetData()
{
	for(int i = 0; i < m_list.count(); i++)
	{
		m_list.at(i)->resetSignalValue();
	}
}

void CustomGraphicsScene::clear()
{
	m_list.clear();
	QGraphicsScene::clear(); // Removes and deletes items
}

void CustomGraphicsScene::removeItem(CustomGraphicsItem *item)
{
	if(item == 0)
		return;

	m_list.removeOne(item);
	QGraphicsScene::removeItem(item);

	delete item;
}

void CustomGraphicsScene::adjustSizeToContent()
{
	setSceneRect(itemsBoundingRect().adjusted(-10,-10,10,10));
}
