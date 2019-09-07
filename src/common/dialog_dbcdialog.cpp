#include "dialog_dbcdialog.h"

DbcDialog::DbcDialog(QList<DbcModel *> dbc_list, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_listWidget = new QListWidget(this);
	//m_listWidget->setSelectionBehavior(QAbstractItemView::SingleSelection);

	m_dbc_list = dbc_list;

	QPushButton *ok = new QPushButton("Ok", this);
	connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton("Cancel", this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	for (int i = 0; i < dbc_list.size(); i++)
		m_listWidget->addItem(new QListWidgetItem(dbc_list.at(i)->getFullPath()));

	m_listWidget->setMinimumWidth(m_listWidget->sizeHintForColumn(0) + 20);


	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(ok);
	buttonLayout->addWidget(cancel);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_listWidget);
	layout->addLayout(buttonLayout);

	setLayout(layout);
}

QString DbcDialog::getDbcPath(QList<DbcModel *> dbc_list, QWidget *parent)
{
	DbcDialog diag(dbc_list, parent);

	if (diag.exec() == QDialog::Accepted)
	{
		return diag.dbcPath();
	}
	else
	{
		return QString();
	}
}

QString DbcDialog::dbcPath()
{
	return m_listWidget->item(m_listWidget->currentIndex().row())->text();
}

DbcModel* DbcDialog::dbc()
{
	if(m_listWidget->currentIndex().row() == -1)
		return 0;

	return m_dbc_list.at(m_listWidget->currentIndex().row());
}

DbcModel* DbcDialog::getDbc(QList<DbcModel *> dbc_list, QWidget *parent)
{
	DbcDialog diag(dbc_list, parent);

	if (diag.exec() == QDialog::Accepted)
	{
		return diag.dbc();
	}
	else
	{
		return 0;
	}
}
