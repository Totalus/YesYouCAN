#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QtWidgets>

struct ExportItem_t
{
	QString sigName;
	bool export_timestamp;
	bool export_raw;
	bool export_phy;
	int hint; // number of columns to export for this item
};

class ExportDialog : public QDialog
{
	Q_OBJECT

	public:
		ExportDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
		void setItemList(QStringList list);
		void setSelectionMode(QAbstractItemView::SelectionMode mode);
		QStringList getSelectedItems();

		QList<ExportItem_t> getSignalsToExport();
		bool exportDisplayedMsgOnly();
		QString getSeperator();
		QString getExportType();

	public slots:
		void updateList();
		void addSignals();
		void removeSignals();
		void itemPressedAction(QTableWidgetItem* item);
		void itemReleasedAction(QTableWidgetItem* item);
		void bringUp();
		void bringDown();

	private:
		QListWidget *m_itemList;
		QTableWidget *m_itemTable;
		QStringList m_signalList;
		QLineEdit* m_filterText;

		// Seperator choice
		QRadioButton *m_tab;
		QRadioButton *m_comma;
		QRadioButton *m_semicolon;

		// Format choice
		QRadioButton *m_csv;
		QRadioButton *m_matlab;

		//Qt::CheckState m_checkStateWhenPressed;
		QTableWidgetItem* m_itemPressed;

};

#endif // EXPORTDIALOG_H
