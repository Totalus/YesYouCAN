#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QtWidgets>

class ItemDialog : public QDialog
{
	Q_OBJECT

	public:
		ItemDialog(QWidget *parent = 0, QString text = QString(), QString buttonAcceptText = "Ok", QString buttonRejectText = "Cancel", Qt::WindowFlags f = 0);
		void setItemList(QStringList list);
		void setSelectionMode(QAbstractItemView::SelectionMode mode);
		QStringList getSelectedItems();

	public slots:
		void accept();

	private:
		QListWidget *m_itemList;
};

#endif // ITEMDIALOG_H
