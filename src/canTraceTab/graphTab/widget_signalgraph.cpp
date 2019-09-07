#include "widget_signalgraph.h"

SignalGraph::SignalGraph(QWidget *parent) : QwtPlot(parent)
{
	m_model = 0;
	m_dbc = 0;

	createMenu();

	setCanvasBackground(Qt::white);
	insertLegend(new QwtLegend());
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach(this);
	grid->setPen(QColor(0,0,0,30));

	canvas()->setCursor(QCursor(Qt::CrossCursor));

	setAxisScale(QwtPlot::yLeft, 0.0, 50.0);
	setAxisAutoScale(QwtPlot::xBottom, true);

	QwtScaleWidget *yleft = axisWidget(QwtPlot::yLeft);
	yleft->installEventFilter(this);

	QwtScaleWidget *xbottom = axisWidget(QwtPlot::xBottom);
	xbottom->installEventFilter(this);

	//QwtPlotZoomer* zoomer = new QwtPlotZoomer(canvas());
	//zoomer->setRubberBandPen(QColor(Qt::black));
	//zoomer->setTrackerPen(QColor(Qt::black));
	//zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );
	//zoomer->setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );

	//QwtPlotPanner *panner = new QwtPlotPanner(canvas());
	//panner->setMouseButton( Qt::MidButton );

	m_leftMousePressed = false;
/*
	QwtPlot plot;
	plot.setTitle( "Plot Demo" );
	plot.setCanvasBackground( Qt::white );
	plot.setAxisScale( QwtPlot::yLeft, 0.0, 10.0);
	plot.insertLegend( new QwtLegend() );

	m_plot = new QwtPlot(this);
	m_plot->setTitle("Titre d'un graph");
	m_plot->setCanvasBackground(Qt::white);
	m_plot->setAxisScale(QwtPlot::yLeft, 0.0, 10.0);
	m_plot->canvas()->setCursor(QCursor(Qt::CrossCursor)); // Change the mouse cursor shape in the graphic area

	//m_plot->insertLegend(new QwtLegend());

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach(m_plot);
	grid->setPen(Qt::lightGray);

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setTitle( "Pixel Count" );
	curve->setPen( Qt::blue, 4 ),
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

	QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
		QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
	curve->setSymbol(symbol);

	curve->setStyle(QwtPlotCurve::Steps);	// Steps drawing
	curve->setCurveAttribute(QwtPlotCurve::Inverted); // To draw steps on the good side

	QPolygonF points;
	points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
		<< QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
		<< QPointF( 4.0, 7.9 ) << QPointF( 4.5, 7.1 );
	curve->setSamples(points);

//	qDebug() << "samples :" << curve->samples();

	curve->attach(m_plot);
*/

}

void SignalGraph::createMenu()
{
	m_menu = new QMenu(this);

	m_menu->addAction(QIcon(":/icons/plus"), "Add signal(s)", this, SLOT(addCurves()));
}

void SignalGraph::setSourceModel(CanTraceModel *model)
{
	m_model = model;
}

void SignalGraph::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;
}

void SignalGraph::addCurves()
{
	if(m_dbc == 0 || m_model == 0)
		return;

	SignalSelectorDialog signal_dialog(this);
	signal_dialog.setItemList(m_dbc->getSignalList());

	if(signal_dialog.exec() != QDialog::Accepted)
		return;

	QStringList signal_list = signal_dialog.getSelectedItems(); // Export signals list
	if(signal_list.count() <= 0)
		return;

	for(int i = 0; i < signal_list.count(); i++)
	{
		QString signalName = signal_list.at(i);

		qDebug() << "Adding signal " << signalName;

		if(m_curves.contains(signalName))
		{
			// Update data
		}
		else
		{
			CanDecoder decoder(m_dbc);
			QPolygonF points = decoder.getSignalPoints(signalName, m_model->getAllMessages());

			if(points.count() == 0)
				return;

			QwtPlotCurve *curve = new QwtPlotCurve(signalName);
			curve->setPen(Qt::blue, 2);
			curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
			curve->setStyle(QwtPlotCurve::Steps);	// Steps drawing
			curve->setCurveAttribute(QwtPlotCurve::Inverted); // To draw steps on the good side

			qDebug() << __LINE__ << curve->legendData().at(0).values();
			qDebug() << __LINE__ << curve->legendData().at(0).value(QwtLegendData::TitleRole);

			//QwtSymbol *symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::blue), QPen(), QSize(4,4) );
			//curve->setSymbol(symbol);
			curve->setSamples(points);

			curve->attach(this);
			replot();


			m_curves[signalName] = curve; // Add curve to list
		}
	}
}



void SignalGraph::contextMenuEvent(QContextMenuEvent *event)
{
	QwtPlot::contextMenuEvent(event); // Send event to items

	if(event->isAccepted())
		return;

	// Rect (mapped to this widget) representing the canvas only (not the legend nor axis)
	QRect canvasRect(canvas()->mapToParent(canvas()->rect().topLeft()), canvas()->mapToParent(canvas()->rect().bottomRight()) );

	if(canvasRect.contains(event->pos()))
	{
		m_menu->exec(mapToGlobal(event->pos()));
		event->accept(); // The scene accepts the event
	}

}

bool SignalGraph::eventFilter(QObject *object, QEvent *event)
{
	// Event for left axis (move and change scale)
	if(object == axisWidget(QwtPlot::yLeft))
	{
		if(event->type() == QMouseEvent::Wheel)
		{
			// Change scale
			QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
			QwtScaleDiv ydiv = axisScaleDiv(QwtPlot::yLeft);
			QwtScaleDraw *ydraw = axisScaleDraw(QwtPlot::yLeft);

			double factor = wheel->delta() / 135.0;

			if(factor < 0)
				factor = -1/factor;

			double mousePosY = ydraw->scaleMap().invTransform(wheel->pos().y());
			double newLowerBound = mousePosY - (mousePosY - ydiv.lowerBound()) * factor;
			double newUpperBound = (ydiv.upperBound() - mousePosY) * factor + mousePosY;

			setAxisScale(yLeft, newLowerBound, newUpperBound);
			replot();
		}
		else if(event->type() == QMouseEvent::MouseButtonPress)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
			{
				m_leftMousePressed = true;
				m_lastMousePosition = mouse->pos();
			}
		}
		else if(event->type() == QMouseEvent::MouseButtonRelease)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
				m_leftMousePressed = false;
		}
		else if(event->type() == QMouseEvent::MouseMove)
		{
			if(m_leftMousePressed)
			{
				// Move scale according to mouse displacement
				QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
				QwtScaleDiv ydiv = axisScaleDiv(QwtPlot::yLeft);
				QwtScaleDraw *ydraw = axisScaleDraw(QwtPlot::yLeft);

				double lastValue = ydraw->scaleMap().invTransform(m_lastMousePosition.y());
				double newValue = ydraw->scaleMap().invTransform(mouse->pos().y());
				double diff = lastValue - newValue;
				m_lastMousePosition = mouse->pos();

				setAxisScale(yLeft, ydiv.lowerBound() + diff, ydiv.upperBound() + diff);
				replot();
			}
		}
		else if(event->type() == QMouseEvent::MouseButtonDblClick)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
			{
				if(m_curves.count() > 0)
				{
					QRectF rect = m_curves.begin().value()->dataRect();

					QMap<QString, QwtPlotCurve*>::const_iterator i;
					for(i = m_curves.cbegin(); i != m_curves.cend(); i++)
					{
						rect |= i.value()->dataRect();
					}

					setAxisScale(yLeft, rect.top(), rect.bottom());
					replot();
				}
			}
		}
	}
	// Event for bottom axis (move and change scale)
	else if(object == axisWidget(QwtPlot::xBottom)) // || object == canvas())
	{
		if(event->type() == QMouseEvent::Wheel)
		{
			// Change scale
			QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
			QwtScaleDiv xdiv = axisScaleDiv(QwtPlot::xBottom);
			QwtScaleDraw *xdraw = axisScaleDraw(QwtPlot::xBottom);

			double factor = wheel->delta() / 135.0;

			if(factor < 0)
				factor = -1/factor;

			double mousePosX = xdraw->scaleMap().invTransform(wheel->pos().x());
			double newLowerBound = mousePosX - (mousePosX - xdiv.lowerBound()) * factor;
			double newUpperBound = (xdiv.upperBound() - mousePosX) * factor + mousePosX;

			setAxisScale(xBottom, newLowerBound, newUpperBound);
			replot();
		}
		else if(event->type() == QMouseEvent::MouseButtonPress)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
			{
				m_leftMousePressed = true;
				m_lastMousePosition = mouse->pos();
			}
		}
		else if(event->type() == QMouseEvent::MouseButtonRelease)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
				m_leftMousePressed = false;
		}
		else if(event->type() == QMouseEvent::MouseMove)
		{
			if(m_leftMousePressed)
			{
				// Move scale according to mouse displacement
				QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
				QwtScaleDiv xdiv = axisScaleDiv(QwtPlot::xBottom);
				QwtScaleDraw *xdraw = axisScaleDraw(QwtPlot::xBottom);

				double lastValue = xdraw->scaleMap().invTransform(m_lastMousePosition.x());
				double newValue = xdraw->scaleMap().invTransform(mouse->pos().x());
				double diff = lastValue - newValue;
				m_lastMousePosition = mouse->pos();

				setAxisScale(xBottom, xdiv.lowerBound() + diff, xdiv.upperBound() + diff);
				replot();
			}
		}
		else if(event->type() == QMouseEvent::MouseButtonDblClick)
		{
			QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

			if(mouse->button() == Qt::LeftButton)
			{
				if(m_curves.count() > 0)
				{
					QRectF rect = m_curves.begin().value()->dataRect();

					QMap<QString, QwtPlotCurve*>::const_iterator i;
					for(i = m_curves.cbegin(); i != m_curves.cend(); i++)
					{
						rect |= i.value()->dataRect();
					}

					setAxisScale(xBottom, rect.left(), rect.right());
					replot();
				}
			}
		}
	}


	return QwtPlot::eventFilter(object, event);
}
