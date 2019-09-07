#ifndef PROJECTDOCK_H
#define PROJECTDOCK_H

#include <QtWidgets>
#include "struct_dbc.h"
#include "document.h"
#include "model_documenttree.h"

class ProjectDock : public QDockWidget
{	
	Q_OBJECT

	public:
		ProjectDock(QWidget *parent, Document *root_document);
		//void update(QList<Document*> doc_list, QList<HwInterface*> iface_list);
		//void update(QList<HwInterface*> iface_list);

	public slots:
		void update();
		void itemDoubleClickAction(QTreeWidgetItem *item, int column);
		void itemActivatedAction(QModelIndex index);

	signals:
		void documentSelectionChanged(QString specific_document_name);
		void documentActivated(Document* doc);

	private:
		QTreeWidget *m_tree;
		Document *m_root_document;

		QTreeView *m_treeView;
		DocumentTreeModel *m_model;

		void addDocument(Document *doc, QTreeWidgetItem *parent);
};

#endif // PROJECTDOCK_H
