#ifndef CUSTOMGRAPHICSSCENE_H
#define CUSTOMGRAPHICSSCENE_H


#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneContextMenuEvent>
#include <QtWidgets/QMenu>

#include "customgraphicsitem.h"

class CustomGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

	public:
		CustomGraphicsScene(QMenu *contextMenu, QWidget* parent = 0);
		void addItem(CustomGraphicsItem* item); // Overloaded of addItem(QGraphicsItem*)
		QList<CustomGraphicsItem*> itemList();
		void removeItem(CustomGraphicsItem* item);

		void adjustSizeToContent(); // Fit scene to content

	public slots:
		void resetData(); // Reset all item's data
		void clear(); // Q_DECL_OVERRIDE;

	protected:
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
		//void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
		//void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
		//void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;


	private:
		QMenu *m_contextMenu;

		QList<CustomGraphicsItem*> m_list; // Required to keep info of CustomGraphicsItem not in QGraphicsItem

};

#endif // CUSTOMGRAPHICSSCENE_H
