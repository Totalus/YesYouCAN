#include "widget_cansignalviewer.h"

#include "application.h"

CanSignalViewerWidget::CanSignalViewerWidget(QWidget *parent, Document *document) : AbstractTabWidget(parent, document)
{
	m_decoder = new CanDecoder(0, this);
	m_device = 0;

	QAction *newObjectAct = new QAction(QIcon(":/icons/plus"), "New object", this);
	QObject::connect(newObjectAct, SIGNAL(triggered(bool)), this, SLOT(addObject()));

	QMenu *sceneMenu = new QMenu(this);
	sceneMenu->addAction(newObjectAct);

	// Widget construction
	m_scene = new CustomGraphicsScene(sceneMenu, this);
	m_view = new CustomGraphicsView(m_scene, this);

	m_view->setInteractive(false);
	m_view->setDragMode(QGraphicsView::RubberBandDrag);

	createMenu();
	createToolbar();

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_toolbar);
	layout->addWidget(m_view);

	setLayout(layout);
}


QMenu* CanSignalViewerWidget::specificMenu()
{
	return m_watcher_menu;
}

void CanSignalViewerWidget::createToolbar()
{
	m_toolbar = new QToolBar(this);
	m_toolbar->addAction(m_setDeviceAct);
	m_toolbar->addAction(m_setDbcAct);
	m_toolbar->addAction(m_resetDataAct);
	m_toolbar->addSeparator();
	m_toolbar->addAction(m_editLayoutAct);
	m_toolbar->addAction(m_saveAndLockLayoutAct);
	m_toolbar->addAction(m_addItemAct);
	m_toolbar->addAction(m_addItemsAct);
	m_toolbar->addAction(m_alignLeftAct);
	m_toolbar->addAction(m_alignRightAct);
	m_toolbar->addAction(m_alignHCenterAct);
	m_toolbar->addAction(m_alignVCenterAct);
	m_toolbar->addAction(m_alignBottomAct);
	m_toolbar->addAction(m_alignTopAct);
	m_toolbar->addAction(m_distributeHAct);
	m_toolbar->addAction(m_distributeVAct);
}

void CanSignalViewerWidget::createMenu()
{
	m_setDeviceAct = new QAction(QIcon(":/icons/disconnected"),"Associate Interface", this);
	m_setDeviceAct->setToolTip("Link to device");
	connect(m_setDeviceAct, SIGNAL(triggered(bool)), this, SLOT(setInterface()));

	m_setDbcAct = new QAction(QIcon(":/icons/database_x"), "Associate DBC", this);
	connect(m_setDbcAct, SIGNAL(triggered()), this, SLOT(setWatcherDbc()));

	m_resetDataAct = new QAction(QIcon(":/icons/reset"), "Reset values", this);
	connect(m_resetDataAct, SIGNAL(triggered(bool)), m_scene, SLOT(resetData()));

	m_editLayoutAct = new QAction(QIcon(":/icons/pen"), "Edit Layout", this);
	connect(m_editLayoutAct, SIGNAL(triggered(bool)), this, SLOT(unlockScene()));

	m_saveAndLockLayoutAct = new QAction(QIcon(":/icons/save_and_lock"), "Save and Lock", this);
	connect(m_saveAndLockLayoutAct, SIGNAL(triggered(bool)), this, SLOT(lockScene()));
	m_saveAndLockLayoutAct->setVisible(false);

	// View actions
	m_zoomFitAct = new QAction(QIcon(":/icons/zoomFit"), "Zoom to fit", this);
	connect(m_zoomFitAct, SIGNAL(triggered(bool)), m_view, SLOT(zoomToFit()));

	// Alignment actions
	m_alignLeftAct = new QAction(QIcon(":/icons/alignLeft"), "Align left", this);
	connect(m_alignLeftAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToLeft()));
	m_alignLeftAct->setVisible(false);

	m_alignRightAct = new QAction(QIcon(":/icons/alignRight"), "Align right", this);
	connect(m_alignRightAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToRight()));
	m_alignRightAct->setVisible(false);

	m_alignHCenterAct = new QAction(QIcon(":/icons/alignCenterH"), "Align horizontal center", this);
	connect(m_alignHCenterAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToHCenter()));
	m_alignHCenterAct->setVisible(false);

	m_alignVCenterAct = new QAction(QIcon(":/icons/alignCenterV"), "Align vertical center", this);
	connect(m_alignVCenterAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToVCenter()));
	m_alignVCenterAct->setVisible(false);

	m_alignBottomAct = new QAction(QIcon(":/icons/alignBottom"), "Align bottom", this);
	connect(m_alignBottomAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToBottom()));
	m_alignBottomAct->setVisible(false);

	m_alignTopAct = new QAction(QIcon(":/icons/alignTop"), "Align top", this);
	connect(m_alignTopAct, SIGNAL(triggered(bool)), this, SLOT(alignItemsToTop()));
	m_alignTopAct->setVisible(false);

	m_addItemAct = new QAction(QIcon(":/icons/plus"), "Add item", this);
	connect(m_addItemAct, SIGNAL(triggered(bool)), this, SLOT(addObject()));
	m_addItemAct->setVisible(false);

	m_addItemsAct = new QAction(QIcon(":/icons/plusplus"), "Add items", this);
	connect(m_addItemsAct, SIGNAL(triggered(bool)), this, SLOT(addObjects()));
	m_addItemsAct->setVisible(false);

	m_distributeHAct = new QAction(QIcon(":/icons/distributeH"), "Distriute horizontally", this);
	connect(m_distributeHAct, SIGNAL(triggered(bool)), this, SLOT(distributeItemsHorizontally()));
	m_distributeHAct->setVisible(false);

	m_distributeVAct = new QAction(QIcon(":/icons/distributeV"), "Distriute vertically", this);
	connect(m_distributeVAct, SIGNAL(triggered(bool)), this, SLOT(distributeItemsVertically()));
	m_distributeVAct->setVisible(false);

	// Menu
	m_watcher_menu = new QMenu(this);
	m_watcher_menu->addAction(m_setDeviceAct);
	m_watcher_menu->addAction(m_setDbcAct);
	m_watcher_menu->addAction(m_resetDataAct);
	m_watcher_menu->addSeparator();
	m_watcher_menu->addAction(QIcon(":/icons/save"),"Save layout", this, SLOT(saveLayout()));
	m_watcher_menu->addAction("Load layout", this, SLOT(loadLayout()));
	m_watcher_menu->addAction(m_editLayoutAct);
	m_watcher_menu->addAction(m_saveAndLockLayoutAct);
	m_watcher_menu->addSeparator();
	m_watcher_menu->addAction(m_zoomFitAct);
}

void CanSignalViewerWidget::addObject()
{
	static bool warningDisplayed = false; // TODO : Use attribute instead of static (static shared between instances)

	if(m_decoder->getDbc() == 0 && !warningDisplayed)
	{
		int answer = QMessageBox::warning(this, "No database associated", "This watcher has no CAN database associated. Signals will not be decoded unless a database is associated.\nAssociate database ?", QMessageBox::Yes, QMessageBox::No);

		if(answer == QMessageBox::Yes)
			setWatcherDbc();

		warningDisplayed = true;
	}

	GraphicsObjectDialog dialog(m_decoder->getDbc());
	if(QDialog::Accepted != dialog.exec())
		return;

	GraphicsObjectParameters_t params = dialog.getParameters();

	// TODO : revoir tout ça
	if(m_decoder->getDbc() != 0)
		if(m_decoder->getDbc()->getSignal(params.signalName) != 0)
			params.signalUnits = m_decoder->getDbc()->getSignal(params.signalName)->getUnits();

	// Create item
	CustomGraphicsItem *item = 0;

	switch(params.itemType)
	{
		case CustomGraphicsItem::TEXT_ITEM:
			item = new TextItem(params);
			break;

		case CustomGraphicsItem::BAR_ITEM:
			item = new BarItem(params);
			break;

		case CustomGraphicsItem::COLOR_SHAPE_ITEM:
			item = new ColorShapeItem(params);
			break;

		default:
			break;
	}

	if(item == 0)
		return;

	item->setParameters(params);

	QObject::connect(m_decoder, SIGNAL(signalValueChanged(SignalValue_t)), item, SLOT(updateSignalValue(SignalValue_t)));

	m_decoder->registerSignal(params.signalName);

	QPoint origin = m_view->mapFromGlobal(QCursor::pos());
	QPointF relativeOrigin = m_view->mapToScene(origin);

	item->setPos(relativeOrigin); // Place under cursor
	m_scene->addItem(item);
	connect(item, SIGNAL(itemRightClicked(CustomGraphicsItem*)), this, SLOT(itemRightClicked(CustomGraphicsItem*)));
	connect(item, SIGNAL(itemDoubleClicked(CustomGraphicsItem*)), this, SLOT(itemDoubleClicked(CustomGraphicsItem*)));
}

void CanSignalViewerWidget::addObjects()
{
	while(m_decoder->getDbc() == 0)
	{
		int answer = QMessageBox::warning(this, "No database associated", "This watcher has no CAN database associated.\nAssociate database ?", QMessageBox::Yes, QMessageBox::No);

		if(answer == QMessageBox::Yes)
			setWatcherDbc();
		else
			return;
	}

	// Get all signals we wish to add

	// Get existing signals
	QStringList signal_list = m_decoder->getDbc()->getSignalList(); // Existing signals (in database)
	signal_list.sort();


	//
	//	Signal selection dialog
	//

	SignalSelectorDialog signal_dialog(this);
	signal_dialog.setItemList(signal_list);

	if(signal_dialog.exec() != QDialog::Accepted)
		return;

	QStringList signal_selected= signal_dialog.getSelectedItems();

	if(signal_selected.count() == 0)
		return;

	GraphicsObjectDialog dialog(m_decoder->getDbc());

	GraphicsObjectParameters_t params = dialog.getParameters(); // Default parameters
	params.signalName = signal_selected.first(); // Set signal name
	dialog.setParameters(params);

	if(QDialog::Accepted != dialog.exec())
		return;

	params = dialog.getParameters();

	// TODO : revoir tout ça
	if(m_decoder->getDbc() != 0)
		if(m_decoder->getDbc()->getSignal(params.signalName) != 0)
			params.signalUnits = m_decoder->getDbc()->getSignal(params.signalName)->getUnits();


	for(int i = 0; i < signal_selected.count(); i++)
	{
		// Create item
		CustomGraphicsItem *item = 0;

		params.signalName = signal_selected.at(i);

		switch(params.itemType)
		{
			case CustomGraphicsItem::TEXT_ITEM:
				item = new TextItem(params);
				break;

			case CustomGraphicsItem::BAR_ITEM:
				item = new BarItem(params);
				break;

			case CustomGraphicsItem::COLOR_SHAPE_ITEM:
				item = new ColorShapeItem(params);
				break;

			default:
				break;
		}

		if(item == 0)
			return;

		item->setParameters(params);

		QObject::connect(m_decoder, SIGNAL(signalValueChanged(SignalValue_t)), item, SLOT(updateSignalValue(SignalValue_t)));

		m_decoder->registerSignal(params.signalName);

		QPoint origin = m_view->mapFromGlobal(QCursor::pos() + QPoint(15, 15) * i);
		QPointF relativeOrigin = m_view->mapToScene(origin);

		item->setPos(relativeOrigin); // Place under cursor
		m_scene->addItem(item);
		connect(item, SIGNAL(itemRightClicked(CustomGraphicsItem*)), this, SLOT(itemRightClicked(CustomGraphicsItem*)));
		connect(item, SIGNAL(itemDoubleClicked(CustomGraphicsItem*)), this, SLOT(itemDoubleClicked(CustomGraphicsItem*)));
	}
}

void CanSignalViewerWidget::setWatcherDbc()
{
	DbcModel* dbc = DbcDialog::getDbc(Application::dbcList(), this);

	if(dbc != 0)
	{
		m_decoder->unregisterAllSignals();
		m_decoder->setDbc(dbc);

		QList<CustomGraphicsItem*> item_list = m_scene->itemList();

		for(int i = 0; i < item_list.size(); i++)
		{
			m_decoder->registerSignal(item_list.at(i)->getSignalName());
		}

		m_setDbcAct->setIcon(QIcon(":/icons/database_ok"));
	}
}

void CanSignalViewerWidget::setInterface()
{
	QStringList iface_str;

	for(int i = 0; i < Application::interfaceList().count(); i++)
	{
		iface_str.append(Application::interfaceList().at(i)->getIdentifier() + " (" + Application::interfaceList().at(i)->getDescription() + ")");
	}

	if(iface_str.isEmpty())
	{
		QMessageBox::information(this, "No Interface", "There is no available interfaces configured. Create a new interface first to associate the interface");
		return;
	}

	ItemDialog dialog(this, "Select interface");
	dialog.setWindowTitle("Existing interfaces");
	dialog.setItemList(iface_str);
	dialog.setSelectionMode(QAbstractItemView::SingleSelection);

	if(dialog.exec() == ItemDialog::Accepted)
	{
		// Set device
		QStringList selection = dialog.getSelectedItems();

		if(!selection.isEmpty())
		{
			// Set the corresponding interface
			for(int i = 0; i < iface_str.count(); i++)
			{
				if(selection.at(0) == iface_str.at(i))
				{
					// Disconnect previous connection with previous device
					if(m_device != 0)
						QObject::disconnect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_decoder, SLOT(newCanMessageReceived(CanTraceMessage)));

					// Assign new device
					m_device = Application::interfaceList().at(i);
					QObject::connect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_decoder, SLOT(newCanMessageReceived(CanTraceMessage)));
					m_device->getDocument()->addChildren(document()); // Add this widget document to the new device document list

					if(!m_device->isCapturing())
					{
						if(!m_device->setCapture(true))
						{
							// TODO : Message box explaining error
							return;
						}
					}

					m_setDeviceAct->setIcon(QIcon(":/icons/connected"));

					return;
				}
			}
		}
	}
}

void CanSignalViewerWidget::saveLayout()
{
	// Document file path
	QString path;
	if(document()->exists())
	{
		path = document()->filePath();
	}
	else
	{
		path = QFileDialog::getSaveFileName(this, document()->fileName(), QString(), "Watcher layout (*.wat)");

		if(path.isEmpty())
			return;
	}


	// Open file
	QFile file(path);

	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, "Error", "Can't write file");
		return;
	}

	QDataStream stream(&file); // Create stream for writing
	stream.setVersion(QDataStream::Qt_5_9);

	quint8 version = 0;
	stream << version; // File version number

	// Get item list
	QList<CustomGraphicsItem*>list = m_scene->itemList();
	quint32 count = list.size();

	// First, save number of items (count) to file
	stream << count;

	// Process to save an item :
	//	- Call serialize function of the item (virtual) :
	//		- Stores the CustomGraphicsItem attributes (including item type)
	//		- Stores the item's specific attributes

	// For each item, save to file
	for(int i = 0; i < list.size(); i++)
	{
		list.at(i)->serialize(stream);
		//stream << (*list.at(i)); // Save item to file
	}

	file.close();

	m_document->setFilePath(path);
	m_document->setSaved(true);
	m_document->setOpened(true);
}

void CanSignalViewerWidget::loadLayout()
{
	openLayout();
}

void CanSignalViewerWidget::openLayout(QString path)
{
	if(path.isEmpty())
	{
		path = QFileDialog::getOpenFileName(this, "Select layout file", QString(), "Watcher layout (*.wat)");

		if(path.isEmpty())
			return;
	}

	QFile file(path);

	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Error reading file", "Can't open file");
		return;
	}

	QDataStream stream(&file);
	stream.setVersion(QDataStream::Qt_5_9);

	quint8 version;
	stream >> version; // File version number

	// Clear scene
	m_scene->clear();

	// Restore items to scene
	quint32 count = 0; // Number of items
	stream >> count;

	// Process to read and add an item :
	//	- Read item type (int)
	//	- Create item according to type
	//	- Call deserialize on this item
	//	- Add item to scene

	for(quint32 i = 0; i < count; i++)
	{
		// Read item type
		int item_type;
		stream >> item_type;

		CustomGraphicsItem *item = 0;
		switch(item_type)
		{
			case CustomGraphicsItem::TEXT_ITEM:
				item = new TextItem(stream);
				break;

			case CustomGraphicsItem::BAR_ITEM:
				item = new BarItem(stream);
				break;

			case CustomGraphicsItem::COLOR_SHAPE_ITEM:
				item = new ColorShapeItem(stream);
				break;

			default: // Item type not supported
				QMessageBox::warning(this, "Error in layout file", "Unsuported item type (" + QString::number(item_type) + "). The item will be ignored. Make sure you have the latest version of the software.");
				i = count; // Will end loop
				break;
		};

		if(item != 0)
		{
			connect(item, SIGNAL(itemRightClicked(CustomGraphicsItem*)), this, SLOT(itemRightClicked(CustomGraphicsItem*)));
			connect(item, SIGNAL(itemDoubleClicked(CustomGraphicsItem*)), this, SLOT(itemDoubleClicked(CustomGraphicsItem*)));
			m_scene->addItem(item);
			m_decoder->registerSignal(item->getSignalName());
			QObject::connect(m_decoder, SIGNAL(signalValueChanged(SignalValue_t)), item, SLOT(updateSignalValue(SignalValue_t)));
		}
	}

	file.close();

	m_document->setFilePath(path);
	m_document->setOpened(true);
}

void CanSignalViewerWidget::unlockScene()
{
	m_view->setInteractive(true);

	m_saveAndLockLayoutAct->setVisible(true);
	m_editLayoutAct->setVisible(false);

	m_alignLeftAct->setVisible(true);
	m_alignRightAct->setVisible(true);
	m_alignHCenterAct->setVisible(true);
	m_alignVCenterAct->setVisible(true);
	m_alignBottomAct->setVisible(true);
	m_alignTopAct->setVisible(true);
	m_addItemAct->setVisible(true);
	m_addItemsAct->setVisible(true);
	m_distributeHAct->setVisible(true);
	m_distributeVAct->setVisible(true);

	m_document->setSaved(false);
}

void CanSignalViewerWidget::lockScene()
{
	m_view->setInteractive(false);
	m_scene->clearSelection();
	m_scene->adjustSizeToContent();

	m_saveAndLockLayoutAct->setVisible(false);
	m_editLayoutAct->setVisible(true);

	m_alignLeftAct->setVisible(false);
	m_alignRightAct->setVisible(false);
	m_alignHCenterAct->setVisible(false);
	m_alignVCenterAct->setVisible(false);
	m_alignBottomAct->setVisible(false);
	m_alignTopAct->setVisible(false);
	m_addItemAct->setVisible(false);
	m_addItemsAct->setVisible(false);
	m_distributeHAct->setVisible(false);
	m_distributeVAct->setVisible(false);

	saveLayout();
}

void CanSignalViewerWidget::itemRightClicked(CustomGraphicsItem *item)
{
	if(item == 0)
		return;

	QMenu menu;
	QAction *edit = menu.addAction(QIcon(":/icons/pen"),"Edit");
	QAction *remove = menu.addAction(QIcon(":icons/close"), "Delete");

	if(m_scene->selectedItems().count() > 0)
	{
		menu.addSeparator();
		menu.addAction(m_alignLeftAct);
		menu.addAction(m_alignRightAct);
		menu.addAction(m_alignHCenterAct);
		menu.addAction(m_alignVCenterAct);
		menu.addAction(m_alignBottomAct);
		menu.addAction(m_alignTopAct);
	}

	QAction* selection = menu.exec(QCursor::pos());


	if(selection == edit)
	{
		GraphicsObjectDialog dialog(m_decoder->getDbc());
		dialog.setParameters(item->getParameters());

		if(QDialog::Accepted != dialog.exec())
			return;

		GraphicsObjectParameters_t params = dialog.getParameters();
		item->setParameters(params);

		item->redraw();
	}
	else if(selection == remove)
	{
		m_scene->removeItem(item);
	}
}

void CanSignalViewerWidget::itemDoubleClicked(CustomGraphicsItem* item)
{
	if(item == 0)
		return;

	GraphicsObjectDialog dialog(m_decoder->getDbc());
	dialog.setParameters(item->getParameters());

	if(QDialog::Accepted != dialog.exec())
		return;

	GraphicsObjectParameters_t params = dialog.getParameters();
	item->setParameters(params);
	item->redraw();
}

void CanSignalViewerWidget::alignItemsToLeft()
{
	alignLeft(m_scene->selectedItems());
}

void CanSignalViewerWidget::alignItemsToRight()
{
	alignRight(m_scene->selectedItems());
}

void CanSignalViewerWidget::alignItemsToHCenter()
{
	alignCenterH(m_scene->selectedItems());
}

void CanSignalViewerWidget::alignItemsToVCenter()
{
	alignCenterV(m_scene->selectedItems());
}

void CanSignalViewerWidget::alignItemsToBottom()
{
	alignBottom(m_scene->selectedItems());
}

void CanSignalViewerWidget::alignItemsToTop()
{
	alignTop(m_scene->selectedItems());
}

void CanSignalViewerWidget::distributeItemsHorizontally()
{
	distributeH(m_scene->selectedItems());
}

void CanSignalViewerWidget::distributeItemsVertically()
{
	distributeV(m_scene->selectedItems());
}
