#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QtWidgets>


class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT

	public:
		CustomGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);

		void wheelEvent(QWheelEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

		void zoomToRect(const QRectF rect);

	public slots:
		void zoomToFit();

};

#endif // CUSTOMGRAPHICSVIEW_H
