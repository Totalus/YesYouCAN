#include "model_documenttree.h"

DocumentTreeModel::DocumentTreeModel(Document *root, QObject *parent) : QAbstractItemModel(parent)
{
	m_root = root;
}

QModelIndex DocumentTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if(!hasIndex(row, column, parent))
			return QModelIndex();

	Document *parentItem;

	if(!parent.isValid())
		parentItem = m_root;
	else
		parentItem = static_cast<Document*>(parent.internalPointer());

	Document *childItem = parentItem->getChildren(row);
	if(childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

Document* DocumentTreeModel::itemAt(QModelIndex &index)
{
	if(!index.isValid())
		return 0;

	Document *doc = static_cast<Document*>(index.internalPointer());
	return doc;
}

QModelIndex DocumentTreeModel::parent(const QModelIndex &index) const
{
	if(!index.isValid())
			return QModelIndex();

	Document *childItem = static_cast<Document*>(index.internalPointer());
	Document *parentItem = childItem->getParent();

	if(parentItem == m_root)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int DocumentTreeModel::rowCount(const QModelIndex &parent) const
{
	Document *parentItem;
	if(parent.column() > 0)
		return 0;

	if(!parent.isValid())
		parentItem = m_root;
	else
		parentItem = static_cast<Document*>(parent.internalPointer());

	return parentItem->getChildren().count();
}

int DocumentTreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant DocumentTreeModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
			return QVariant();


	Document *doc = static_cast<Document*>(index.internalPointer());
	switch(role)
	{
		case Qt::DisplayRole:
			return doc->documentName();
			break;

		case Qt::DecorationRole:
			return doc->getIcon();
			break;

		default:
			return QVariant();
	}

	if(role == Qt::DisplayRole)
		return QVariant();

}

