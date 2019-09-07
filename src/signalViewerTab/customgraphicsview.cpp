#include "customgraphicsview.h"


#include <QDebug>

CustomGraphicsView::CustomGraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // More intuitive zooming (towards mouse)
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
	if(QApplication::keyboardModifiers() == Qt::ControlModifier) // Ctrl pressed
	{
		qreal zoomFactor = event->delta() / 1000.0;
		scale(1 + zoomFactor,1 + zoomFactor);

		event->accept();
		return;
	}

	QGraphicsView::wheelEvent(event);
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void CustomGraphicsView::zoomToRect(const QRectF rect)
{
	fitInView(rect, Qt::KeepAspectRatio);
}

void CustomGraphicsView::zoomToFit()
{
	zoomToRect(scene()->sceneRect());
}
