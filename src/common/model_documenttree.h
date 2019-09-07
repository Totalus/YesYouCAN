#ifndef MODEL_DOCUMENTTRE_H
#define MODEL_DOCUMENTTRE_H

#include <QtWidgets>
#include "document.h"


class DocumentTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		DocumentTreeModel(Document *root, QObject *parent = 0);

		Document* itemAt(QModelIndex &index);

		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		//QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;

		//Qt::ItemFlags flags(const QModelIndex &index) const override;

	private:
		Document *m_root; // Root document
};

#endif // MODEL_DOCUMENTTRE_H
