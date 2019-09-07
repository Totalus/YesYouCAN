
#include "dialog_exportdialog.h"



ExportDialog::ExportDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_itemPressed = 0;

	setWindowTitle("Export Signals");
	setWindowModality(Qt::NonModal);

	//
	// Widgets and drawing
	//

	m_itemList = new QListWidget(this);
	m_itemList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_itemList->setMinimumWidth(200);


	m_itemTable = new QTableWidget(0, 4, this); // Columns : signal name, timestamp, raw value, formatted (physical) value
	m_itemTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	// Headers (hide)
	m_itemTable->horizontalHeader()->hide();
	m_itemTable->verticalHeader()->hide();
	// Sizing
	m_itemTable->setMinimumWidth(400);
	m_itemTable->setColumnWidth(0, 250);
	m_itemTable->setColumnWidth(1, 60);
	m_itemTable->setColumnWidth(2, 60);
	m_itemTable->setColumnWidth(3, 80);
	// Connection behaviours (for check box multiple triggers)
	connect(m_itemTable, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(itemPressedAction(QTableWidgetItem*)));
	connect(m_itemTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemReleasedAction(QTableWidgetItem*)));


	QPushButton *button_accept = new QPushButton("Export", this);
	connect(button_accept, SIGNAL(clicked(bool)), this, SLOT(accept()));
	QPushButton *button_cancel = new QPushButton("Cancel", this);
	connect(button_cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));


	QPushButton *button_add_signals = new QPushButton("Add >>", this);
	connect(button_add_signals, SIGNAL(clicked()), this, SLOT(addSignals()));
	QPushButton *button_remove_signals = new QPushButton("Remove <<", this);
	connect(button_remove_signals, SIGNAL(clicked()), this, SLOT(removeSignals()));
	QPushButton *button_up = new QPushButton("Move Up", this);
	connect(button_up, SIGNAL(clicked(bool)), this, SLOT(bringUp()));
	QPushButton *button_down = new QPushButton("Move Down", this);
	connect(button_down, SIGNAL(clicked(bool)), this, SLOT(bringDown()));


	m_filterText = new QLineEdit(this);
	m_filterText->setPlaceholderText("Filter");
	QObject::connect(m_filterText, SIGNAL(textChanged(QString)), this, SLOT(updateList()));


	// Seperator related
	m_tab = new QRadioButton("Tab", this);
	m_tab->setChecked(true);
	m_comma = new QRadioButton("Comma (,)", this);
	m_semicolon = new QRadioButton("Semicolon (;)", this);

	QGroupBox *seperator_box = new QGroupBox("Column seperator", this);

	QFormLayout *seperator_layout = new QFormLayout();
	seperator_layout->addWidget(m_tab);
	seperator_layout->addWidget(m_comma);
	seperator_layout->addWidget(m_semicolon);

	seperator_box->setLayout(seperator_layout);

	// Format related
	m_csv = new QRadioButton("CSV", this);
	m_csv->setChecked(true);
	connect(m_csv, SIGNAL(toggled(bool)), seperator_box, SLOT(setVisible(bool)));

	m_matlab = new QRadioButton("Matlab");


	QGroupBox *format_box = new QGroupBox("Output format", this);

	QFormLayout *format_layout = new QFormLayout();
	format_layout->addWidget(m_csv);
	format_layout->addWidget(m_matlab);

	format_box->setLayout(format_layout);


	QGridLayout *content_layout = new QGridLayout();
	// Column 0
	content_layout->addWidget(new QLabel("Signal List", this), 0, 0, 1, 1);
	content_layout->addWidget(m_filterText, 1,0);
	content_layout->addWidget(m_itemList, 2, 0, 6, 1);
	// Column 1
	content_layout->addWidget(button_add_signals, 2, 1);
	content_layout->addWidget(button_remove_signals, 3, 1);
	content_layout->addWidget(button_up, 4, 1);
	content_layout->addWidget(button_down, 5, 1);
	content_layout->addWidget(format_box, 6, 1);
	content_layout->addWidget(seperator_box, 7,1);
	// Column 2
	content_layout->addWidget(new QLabel("Signals to Export", this), 1, 2, 1, 1);
	content_layout->addWidget(m_itemTable, 2, 2, 6, 1);
	// Column 3
	content_layout->addWidget(button_accept,2, 3);
	content_layout->addWidget(button_cancel, 3, 3);

	// Stretch behaviour
	content_layout->setColumnMinimumWidth(1, 150);
	content_layout->setColumnStretch(2,2);
	content_layout->setColumnStretch(0,1);
	content_layout->setRowStretch(7,5);

	setLayout(content_layout);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar
}

void ExportDialog::setItemList(QStringList list)
{
	m_signalList = list;

	m_itemList->clear();
	m_itemList->addItems(list);
}

QStringList ExportDialog::getSelectedItems()
{
	QStringList list;
	QList<QListWidgetItem*> item_list = m_itemList->selectedItems();

	for(int i = 0; i < item_list.count(); i++)
	{
		list << item_list.at(i)->text();
	}

	return list;
}

void ExportDialog::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
	m_itemList->setSelectionMode(mode);
}

void ExportDialog::updateList()
{
	m_itemList->clear();
	m_signalList.sort();
	m_itemList->addItems(m_signalList.filter(m_filterText->text(), Qt::CaseInsensitive));
}

void ExportDialog::addSignals()
{
	// Get selected items
	QStringList list;
	QList<QListWidgetItem*> item_list = m_itemList->selectedItems();

	for(int i = 0; i < item_list.count(); i++)
	{
		list << item_list.at(i)->text();
		m_signalList.removeOne(list.at(i)); // Remove item from signal list

		// Add to export signal table
		QTableWidgetItem *checkBox1 = new QTableWidgetItem();
		checkBox1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		checkBox1->setCheckState(Qt::Unchecked);
		checkBox1->setTextAlignment(Qt::AlignCenter);

		QTableWidgetItem *sigName = new QTableWidgetItem(list.at(i));
		sigName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		sigName->setToolTip(list.at(i));

		QTableWidgetItem *tstamp = new QTableWidgetItem("Time");
		tstamp->setToolTip("Message timestamp");
		tstamp->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
		tstamp->setCheckState(Qt::Checked);

		QTableWidgetItem *raw = new QTableWidgetItem("Raw");
		raw->setToolTip("Raw value (from message data directly)");
		raw->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
		raw->setCheckState(Qt::Checked);

		QTableWidgetItem *phy = new QTableWidgetItem("Physical");
		phy->setToolTip("Factor and offset an units applied");
		phy->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
		phy->setCheckState(Qt::Checked);

		int row = m_itemTable->rowCount();
		m_itemTable->setRowCount(row + 1);
		m_itemTable->setItem(row, 0, sigName);
		m_itemTable->setItem(row, 1, tstamp);
		m_itemTable->setItem(row, 2, raw);
		m_itemTable->setItem(row, 3, phy);

		m_itemTable->resizeRowToContents(row); // Resize row height
	}

	// Resize first column
	m_itemTable->resizeColumnToContents(0);
	m_itemTable->setColumnWidth(0, (m_itemTable->columnWidth(0) > 150) ? m_itemTable->columnWidth(0)+5 : 150 );

	updateList();
}

void ExportDialog::removeSignals()
{
	QList<QTableWidgetItem *> selection = m_itemTable->selectedItems();
	QList<QTableWidgetItem *> items_col0; // Items at column 0

	for(int i = 0; i < selection.size(); i++)
	{
		QTableWidgetItem* item_at_col0 = m_itemTable->item(selection.at(i)->row(), 0);

		if(!items_col0.contains(item_at_col0))
			items_col0.append(m_itemTable->item(selection.at(i)->row(), 0));
	}

	for(int i = 0; i < items_col0.size(); i++)
	{
		int row = items_col0.at(i)->row(); // Selected item row
		m_signalList << m_itemTable->item(row, 0)->text(); // Signal name (put signal back in signal list)
		m_itemTable->removeRow(row); // Remove from export list
	}

	// Resize first column
	m_itemTable->resizeColumnToContents(0);
	m_itemTable->setColumnWidth(0, (m_itemTable->columnWidth(0) > 150) ? m_itemTable->columnWidth(0)+5 : 150 );

	updateList();
}

void ExportDialog::itemPressedAction(QTableWidgetItem* item)
{
	//m_checkStateWhenPressed = item->checkState();

	m_itemPressed = item; // To detect if it is a checkbox action or not
}

void ExportDialog::itemReleasedAction(QTableWidgetItem* itemReleased)
{
	if(m_itemPressed == itemReleased)
	{
		// Not a checkbox action (checkbox actions does not trigger a press signal)
		m_itemPressed = 0;
		return;
	}

	if(itemReleased->column() == 0)
		return; // Released on first item

	Qt::CheckState newState = itemReleased->checkState();

	QList<QTableWidgetItem *> selection = m_itemTable->selectedItems(); // All selected items

	for(int i = 0; i < selection.size(); i++)
	{
		if(selection.at(i)->column() >= 1)
			selection.at(i)->setCheckState(newState);
	}

	m_itemPressed = 0;
}

void ExportDialog::bringDown()
{
	int row = m_itemTable->currentRow();

	if(row == m_itemTable->rowCount() - 1) // Last row
		return;

	int newRow = row + 2;

	m_itemTable->insertRow(newRow);

	m_itemTable->setItem(newRow, 0, m_itemTable->takeItem(row, 0));
	m_itemTable->setItem(newRow, 1, m_itemTable->takeItem(row, 1));
	m_itemTable->setItem(newRow, 2, m_itemTable->takeItem(row, 2));
	m_itemTable->setItem(newRow, 3, m_itemTable->takeItem(row, 3));

	m_itemTable->resizeRowToContents(newRow);
	m_itemTable->removeRow(row);

	m_itemTable->selectRow(newRow - 1);
	m_itemTable->setFocus();
}

void ExportDialog::bringUp()
{
	int row = m_itemTable->currentRow();

	if(row < 1) // First row
		return;

	int newRow = row - 1;
	row = row + 1; // Rows all shifted down

	m_itemTable->insertRow(newRow);

	m_itemTable->setItem(newRow, 0, m_itemTable->takeItem(row, 0));
	m_itemTable->setItem(newRow, 1, m_itemTable->takeItem(row, 1));
	m_itemTable->setItem(newRow, 2, m_itemTable->takeItem(row, 2));
	m_itemTable->setItem(newRow, 3, m_itemTable->takeItem(row, 3));

	m_itemTable->resizeRowToContents(newRow);
	m_itemTable->removeRow(row);

	m_itemTable->selectRow(newRow);
	m_itemTable->setFocus();
}


QList<ExportItem_t> ExportDialog::getSignalsToExport()
{
	QList<ExportItem_t> list;

	for(int i = 0; i < m_itemTable->rowCount(); i++)
	{
		ExportItem_t a;

		a.sigName = m_itemTable->item(i, 0)->text(); // Signal name
		a.export_timestamp = (m_itemTable->item(i, 1)->checkState() == Qt::Checked); // Time stamp export
		a.export_raw = (m_itemTable->item(i, 2)->checkState() == Qt::Checked);	// Raw value export
		a.export_phy = (m_itemTable->item(i, 3)->checkState() == Qt::Checked); // Physical/Formatted value
		a.hint = a.export_phy + a.export_raw + a.export_timestamp;

		list << a;
	}

	return list;
}

QString ExportDialog::getSeperator()
{
	if(m_tab->isChecked())
		return "\t";
	else if (m_comma->isChecked())
		return ",";
	else if (m_semicolon->isChecked())
		return ";";
	else
		return ",";
}

QString ExportDialog::getExportType()
{
	if(m_csv->isChecked())
		return "csv";
	else if (m_matlab->isChecked())
		return "matlab";
	else
		return "csv";
}
