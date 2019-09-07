#include "signalselectordialog.h"

SignalSelectorDialog::SignalSelectorDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setWindowTitle("Select signals");
	setWindowModality(Qt::NonModal);

	//
	// Widgets and drawing
	//

	m_itemList = new QListWidget(this);
	m_itemList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_itemList->setMinimumWidth(200);

	m_itemList_selected = new QListWidget(this);
	m_itemList_selected->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_itemList_selected->setMinimumWidth(200);
	m_itemList_selected->setSortingEnabled(true);

	QPushButton *button_accept = new QPushButton("Ok", this);
	connect(button_accept, SIGNAL(clicked(bool)), this, SLOT(accept()));
	QPushButton *button_cancel = new QPushButton("Cancel", this);
	connect(button_cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QPushButton *button_add_signals = new QPushButton("Add >>", this);
	connect(button_add_signals, SIGNAL(clicked()), this, SLOT(addSignals()));
	QPushButton *button_remove_signals = new QPushButton("Remove <<", this);
	connect(button_remove_signals, SIGNAL(clicked()), this, SLOT(removeSignals()));
	//QPushButton *button_up = new QPushButton("Move Up", this);
	//connect(button_up, SIGNAL(clicked(bool)), this, SLOT(bringUp()));
	//QPushButton *button_down = new QPushButton("Move Down", this);
	//connect(button_down, SIGNAL(clicked(bool)), this, SLOT(bringDown()));


	m_filterText = new QLineEdit(this);
	m_filterText->setPlaceholderText("Filter");
	QObject::connect(m_filterText, SIGNAL(textChanged(QString)), this, SLOT(updateList()));


	QGridLayout *content_layout = new QGridLayout();
	// Column 0
	content_layout->addWidget(new QLabel("Signal List", this), 0, 0, 1, 1);
	content_layout->addWidget(m_filterText, 1,0);
	content_layout->addWidget(m_itemList, 2, 0, 6, 1);
	// Column 1
	content_layout->addWidget(button_add_signals, 2, 1);
	content_layout->addWidget(button_remove_signals, 3, 1);
	//content_layout->addWidget(button_up, 4, 1);
	//content_layout->addWidget(button_down, 5, 1);
	// Column 2
	content_layout->addWidget(new QLabel("Selected signals", this), 1, 2, 1, 1);
	content_layout->addWidget(m_itemList_selected, 2, 2, 6, 1);
	// Column 3
	content_layout->addWidget(button_accept,2, 3);
	content_layout->addWidget(button_cancel, 3, 3);

	// Stretch behaviour
	content_layout->setColumnMinimumWidth(1, 150);
	content_layout->setColumnStretch(2,2);
	content_layout->setColumnStretch(0,1);
	content_layout->setRowStretch(7,5);

	setLayout(content_layout);

	setMinimumHeight(400);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar
}



void SignalSelectorDialog::updateList()
{
	m_itemList->clear();
	m_signalList.sort();
	m_itemList->addItems(m_signalList.filter(m_filterText->text(), Qt::CaseInsensitive));
}

void SignalSelectorDialog::addSignals()
{
	// Get selected items
	QList<QListWidgetItem*> item_list = m_itemList->selectedItems();

	for(int i = 0; i < item_list.count(); i++)
	{
		QString signalName = item_list.at(i)->text();
		m_itemList_selected->addItem(signalName); // Add signal to selected items
		m_signalList.removeOne(signalName); // Remove item from signal list
	}

	updateList();
}

void SignalSelectorDialog::removeSignals()
{
	// Get selected items
	QList<QListWidgetItem*> item_list = m_itemList_selected->selectedItems();

	for(int i = 0; i < item_list.count(); i++)
	{
		QString signalName = item_list.at(i)->text();

		m_signalList.append(signalName); // Add to unselected list
		delete item_list.at(i); // Remove from selected list
	}

	updateList();
}


void SignalSelectorDialog::bringDown()
{

}

void SignalSelectorDialog::bringUp()
{

}

QStringList SignalSelectorDialog::getSelectedItems()
{
	QStringList items;
	for(int i = 0; i < m_itemList_selected->count(); i++)
	{
		items.append(m_itemList_selected->item(i)->text());
	}

	return items;
}

void SignalSelectorDialog::setItemList(QStringList list)
{
	m_signalList = list;
	updateList();
}
