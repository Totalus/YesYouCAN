#include "widget_abstracttab.h"

AbstractTabWidget::AbstractTabWidget(QWidget *parent , Document* document) : QWidget(parent)
{
	m_document = document;
}

QMenu* AbstractTabWidget::specificMenu()
{
	return 0; // Default implementation : no specific menu
}


QStatusBar* AbstractTabWidget::specificStatusBar()
{
	return 0; // Default implementation : no specific status bar
}

QToolBar* AbstractTabWidget::specificToolBar()
{
	return 0; // Default implementation : no specific tool bar
}


Document* AbstractTabWidget::document()
{
	return m_document;
}
