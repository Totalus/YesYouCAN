#include "dock_project.h"

ProjectDock::ProjectDock(QWidget* parent, Document *root_document) : QDockWidget("Documents", parent)
{
	m_root_document = root_document;

	m_treeView = new QTreeView(this);
	m_treeView->header()->hide();
	m_model = new DocumentTreeModel(root_document, this);
	m_treeView->setModel(m_model);

	connect(m_treeView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivatedAction(QModelIndex)));
	//connect(m_treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(...));

	m_tree = new QTreeWidget(this);
	m_tree->setHeaderHidden(true);
	connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDoubleClickAction(QTreeWidgetItem*,int)));

	//setWidget(m_tree);

	m_tree->setVisible(false);
	setWidget(m_treeView);
}

void ProjectDock::itemActivatedAction(QModelIndex index)
{
	if(!index.isValid())
		return;

	Document* doc = m_model->itemAt(index); // Get item activated

	if(doc != 0)
		emit documentActivated(doc);
}

/*
void ProjectDock::update(QList<Document*> doc_list, QList<HwInterface *> iface_list)
{
	update(iface_list);
	return;

	m_tree->clear(); // Remove all items

	QTreeWidgetItem *ifaces = 0;
	QTreeWidgetItem *dbc_windows = 0;
	QTreeWidgetItem *canTrace_windows = 0;
	QTreeWidgetItem *canFixedTrace_windows = 0;


	QTreeWidgetItem *item;

	// Interface list
	for(int i = 0; i < iface_list.count(); i++)
	{
		if(ifaces == 0)
		{
			ifaces = new QTreeWidgetItem();
			ifaces->setText(0, "Hardware Interfaces");
			m_tree->addTopLevelItem(ifaces);
			ifaces->setExpanded(true);
		}

		item = new QTreeWidgetItem(CAN_INTERFACE);
		item->setText(0, iface_list.at(i)->getIdentifier() + " (" + iface_list.at(i)->interfaceTypeStr() + ")");
		item->setToolTip(0, iface_list.at(i)->getDescription());
		ifaces->addChild(item);

		for(int j = 0; j < iface_list.at(i)->getDocument()->getChildren().count(); j++)
		{
			addDocument(iface_list.at(i)->getDocument()->getChildren().at(j), item);
		}

	}

	// Document list
	for(int i = 0; i < doc_list.count(); i++)
	{
		switch(doc_list.at(i)->getType())
		{

			case CAN_TRACE:
				if(canTrace_windows == 0)
				{
					// Create CAN Trace list tree item
					canTrace_windows = new QTreeWidgetItem();
					canTrace_windows->setText(0, "CAN Traces");
					m_tree->addTopLevelItem(canTrace_windows);
					canTrace_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), canTrace_windows);


				break;


			case CAN_DBC:
				if(dbc_windows == 0)
				{
					dbc_windows = new QTreeWidgetItem();
					dbc_windows->setText(0, "CAN Databases");
					m_tree->addTopLevelItem(dbc_windows);
					dbc_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), dbc_windows);


				break;

			case CAN_FIXED_TRACE:
				if(canFixedTrace_windows == 0)
				{
					canFixedTrace_windows = new QTreeWidgetItem();
					canFixedTrace_windows->setText(0, "CAN Messages");
					m_tree->addTopLevelItem(canFixedTrace_windows);
					canFixedTrace_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), canFixedTrace_windows);



				break;

			default:
				break;

		}
	}
}

void ProjectDock::update(QList<HwInterface *> iface_list)
{
	m_tree->clear(); // Remove all items

	QTreeWidgetItem *ifaces = 0;
	QTreeWidgetItem *dbc_windows = 0;
	QTreeWidgetItem *canTrace_windows = 0;
	QTreeWidgetItem *canFixedTrace_windows = 0;


	QTreeWidgetItem *item;

	// Interface list
	for(int i = 0; i < iface_list.count(); i++)
	{
		if(ifaces == 0)
		{
			ifaces = new QTreeWidgetItem();
			ifaces->setText(0, "Hardware Interfaces");
			m_tree->addTopLevelItem(ifaces);
			ifaces->setExpanded(true);
		}

		item = new QTreeWidgetItem(CAN_INTERFACE);
		item->setText(0, iface_list.at(i)->getIdentifier() + " (" + iface_list.at(i)->interfaceTypeStr() + ")");
		item->setToolTip(0, iface_list.at(i)->getDescription());
		ifaces->addChild(item);

		for(int j = 0; j < iface_list.at(i)->getDocument()->getChildren().count(); j++)
		{
			addDocument(iface_list.at(i)->getDocument()->getChildren().at(j), item);
		}

	}

	// Document list
	QList<Document*> doc_list = m_root_document->getChildren();

	for(int i = 0; i < doc_list.count(); i++)
	{
		switch(doc_list.at(i)->getType())
		{

			case CAN_TRACE:
				if(canTrace_windows == 0)
				{
					// Create CAN Trace list tree item
					canTrace_windows = new QTreeWidgetItem();
					canTrace_windows->setText(0, "CAN Traces");
					m_tree->addTopLevelItem(canTrace_windows);
					canTrace_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), canTrace_windows);

				break;


			case CAN_DBC:
				if(dbc_windows == 0)
				{
					dbc_windows = new QTreeWidgetItem();
					dbc_windows->setText(0, "CAN Databases");
					m_tree->addTopLevelItem(dbc_windows);
					dbc_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), dbc_windows);

				break;

			case CAN_FIXED_TRACE:
				if(canFixedTrace_windows == 0)
				{
					canFixedTrace_windows = new QTreeWidgetItem();
					canFixedTrace_windows->setText(0, "CAN Messages");
					m_tree->addTopLevelItem(canFixedTrace_windows);
					canFixedTrace_windows->setExpanded(true);
				}

				addDocument(doc_list.at(i), canFixedTrace_windows);

				break;

			default:
				break;

		}
	}
}
*/

void ProjectDock::update()
{
	m_treeView->reset();
	m_treeView->expandAll();

	m_tree->clear(); // Remove all items

	QTreeWidgetItem *orphans = 0;
	QTreeWidgetItem *ifaces = 0;
	QTreeWidgetItem *dbc_windows = 0;

	QTreeWidgetItem *item;

	// Document list
	QList<Document*> doc_list = m_root_document->getChildren();

	for(int i = 0; i < doc_list.count(); i++)
	{
		Document *doc = doc_list.at(i);

		switch(doc_list.at(i)->getType())
		{
			case CAN_INTERFACE:
				if(doc->getHwInterface() == 0)
					qWarning() << "Invalid hardware interface";

				if(ifaces == 0)
				{
					ifaces = new QTreeWidgetItem();
					ifaces->setText(0, "Hardware Interfaces");
					ifaces->setExpanded(true);
				}

				item = new QTreeWidgetItem(CAN_INTERFACE);
				item->setText(0, doc->getHwInterface()->getIdentifier() + " (" + doc->getHwInterface()->interfaceTypeStr() + ")");
				item->setToolTip(0, doc->getHwInterface()->getDescription());
				item->setIcon(0, doc->getIcon());
				ifaces->addChild(item);
				item->setExpanded(true);

				for(int j = 0; j < doc->getChildren().count(); j++)
				{
					addDocument(doc->getChildren().at(j), item);
				}
				break;

			case CAN_DBC:
				if(dbc_windows == 0)
				{
					dbc_windows = new QTreeWidgetItem();
					dbc_windows->setText(0, "CAN Databases");
				}

				addDocument(doc_list.at(i), dbc_windows);

				break;

			default: // Orphan document
				if(orphans == 0)
				{
					// Create CAN Trace list tree item
					orphans = new QTreeWidgetItem();
					orphans->setText(0, "Orphan document");
				}

				addDocument(doc_list.at(i), orphans);

				//qWarning() << "Unknown document type " << doc_list.at(i)->getType();
				break;
		}
	}

	if(ifaces != 0)
	{
		m_tree->addTopLevelItem(ifaces);
		ifaces->setExpanded(true);
	}
	if(dbc_windows != 0)
	{
		m_tree->addTopLevelItem(dbc_windows);
		dbc_windows->setExpanded(true);
	}
	if(orphans != 0)
	{
		m_tree->addTopLevelItem(orphans);
		orphans->setExpanded(true);
	}
}

void ProjectDock::addDocument(Document* doc, QTreeWidgetItem *parent)
{
	QTreeWidgetItem *item = new QTreeWidgetItem( QStringList() << doc->fileName()  , doc->getType());
	item->setToolTip(0, doc->filePath());
	item->setIcon(0, doc->getIcon());
	item->setExpanded(true);

	parent->addChild(item);

	for(int i = 0; i < doc->getChildren().count(); i++)
	{
		addDocument(doc->getChildren().at(i), item); // Recursive call
	}

	parent->setExpanded(true);
}


void ProjectDock::itemDoubleClickAction(QTreeWidgetItem *item, int column)
{
	if(item == 0)
		return;

	if(item->type() == 0)
		return;

	emit documentSelectionChanged(item->text(column));
}

