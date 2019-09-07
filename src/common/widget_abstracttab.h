#ifndef ABSTRACTTABWIDGET_H
#define ABSTRACTTABWIDGET_H

#include <QtWidgets>

class Document;


// Abstract class for all widget placed in a main window or auxiliary window tab widget

class AbstractTabWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit AbstractTabWidget(QWidget *parent = 0, Document *document = 0);

		virtual QMenu* specificMenu();				// Menu specific to that widget
		virtual QStatusBar* specificStatusBar();	// Status bar for this object
		virtual QToolBar* specificToolBar();		// Tool bar for this object

		Document* document();				// Get related document object

	protected:
		Document *m_document; // Related Document object

};

#endif // ABSTRACTTABWIDGET_H
