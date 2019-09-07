#ifndef SIGNALSELECTORDIALOG_H
#define SIGNALSELECTORDIALOG_H

#include <QtWidgets>


class SignalSelectorDialog : public QDialog
{
	Q_OBJECT

	public:
		SignalSelectorDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
		void setItemList(QStringList list);
		QStringList getSelectedItems();

	public slots:
		void updateList();
		void addSignals();
		void removeSignals();
		void bringUp();
		void bringDown();

	private:
		QLineEdit* m_filterText;
		QListWidget *m_itemList;
		QStringList m_signalList;

		QListWidget *m_itemList_selected;




};

#endif // SIGNALSELECTORDIALOG_H
