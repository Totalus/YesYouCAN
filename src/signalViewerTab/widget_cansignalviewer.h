#ifndef SIGNALVIEWERWIDGET_H
#define SIGNALVIEWERWIDGET_H

#include "widget_abstracttab.h"
#include "customgraphicsscene.h"
#include "document.h"
#include "candecoder.h"
#include "dialog_graphicsobject.h"
#include "customgraphicsview.h"
#include "signalselectordialog.h"

// Items
#include "textitem.h"
#include "baritem.h"
#include "colorshapeitem.h"

#include <QtWidgets/QGraphicsView>

class CanSignalViewerWidget : public AbstractTabWidget
{
	Q_OBJECT

	public:
		CanSignalViewerWidget(QWidget *parent, Document *document);

		QMenu* specificMenu();

		void openLayout(QString path = QString());		// Open layout from file

	public slots:
		void saveLayout();		// Save layout to file
		void loadLayout();		// Load layout from file

		void addObject();		// Add object to graphics scene
		void addObjects();		// Add multiple objects to the graphics scene
		void setWatcherDbc();
		void setInterface();

		void unlockScene();
		void lockScene();

		void itemRightClicked(CustomGraphicsItem* item);
		void itemDoubleClicked(CustomGraphicsItem* item);

		// Alignment slots
		void alignItemsToLeft();
		void alignItemsToRight();
		void alignItemsToHCenter();
		void alignItemsToVCenter();
		void alignItemsToBottom();
		void alignItemsToTop();
		void distributeItemsHorizontally();
		void distributeItemsVertically();

	private:
		CustomGraphicsView *m_view;
		CustomGraphicsScene *m_scene;
		HwInterface *m_device;

		CanDecoder *m_decoder; // Decoder object (receives raw msg, emits signal value changed)

		QMenu* m_watcher_menu;

		// Tab actions
		QAction* m_setDeviceAct;
		QAction* m_setDbcAct;
		QAction* m_saveAndLockLayoutAct;
		QAction* m_editLayoutAct;
		QAction* m_resetDataAct;

		// Scene/View actions
		QAction* m_zoomFitAct;

		// Item actions
		QAction* m_addItemAct;
		QAction* m_addItemsAct;
		//QAction* m_removeItemAct;
		//QAction* m_editItemAct;
		//QAction* m_duplicateItem;
		QAction* m_alignLeftAct;
		QAction* m_alignRightAct;
		QAction* m_alignHCenterAct;
		QAction* m_alignVCenterAct;
		QAction* m_alignBottomAct;
		QAction* m_alignTopAct;
		QAction* m_distributeVAct;
		QAction* m_distributeHAct;


		QToolBar *m_toolbar;

		void createMenu();
		void createToolbar();
};

#endif // SIGNALVIEWERWIDGET_H
