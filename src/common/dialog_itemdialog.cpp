#include "dialog_itemdialog.h"



ItemDialog::ItemDialog(QWidget *parent, QString text, QString buttonAcceptText, QString buttonRejectText, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_itemList = new QListWidget(this);
	m_itemList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QPushButton *button_accept = new QPushButton(buttonAcceptText, this);
	connect(button_accept, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *button_cancel = new QPushButton(buttonRejectText, this);
	connect(button_cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QHBoxLayout *button_layout = new QHBoxLayout();
	button_layout->addWidget(button_accept);
	button_layout->addWidget(button_cancel);

	QVBoxLayout *layout = new QVBoxLayout();

	if(!text.isEmpty())
		layout->addWidget(new QLabel(text, this));

	layout->addWidget(m_itemList);
	layout->addLayout(button_layout);
	setLayout(layout);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar
}

void ItemDialog::setItemList(QStringList list)
{
	m_itemList->clear();
	m_itemList->addItems(list);
}

QStringList ItemDialog::getSelectedItems()
{
	QStringList list;
	QList<QListWidgetItem*> item_list = m_itemList->selectedItems();

	for(int i = 0; i < item_list.count(); i++)
	{
		list << item_list.at(i)->text();
	}

	return list;
}

void ItemDialog::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
	m_itemList->setSelectionMode(mode);
}

void ItemDialog::accept()
{
	if(getSelectedItems().isEmpty())
	{
		QMessageBox::information(this, "No Item Selected", "You need to make at least one selection");
		return;
	}

	QDialog::accept();
}
