#include "view_customtransmittable.h"

CustomTransmitTableView::CustomTransmitTableView(QWidget *parent) : QTableView(parent)
{

}

void CustomTransmitTableView::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Space)
	{
		emit spacePressed(selectedIndexes());
	}

	QTableView::keyPressEvent(event);
}
