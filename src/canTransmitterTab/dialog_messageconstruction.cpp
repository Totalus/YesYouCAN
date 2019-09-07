#include "dialog_messageconstruction.h"


MessageConstructionDialog::MessageConstructionDialog(CanTxMessageObject *msgObject, DbcModel *dbc, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	m_msgObject = msgObject;						// Store to update later
	m_transmitEnable = m_msgObject->getTransmit();	// Save parameter
	m_msgObject->setTransmit(false);				// Stop transmission when modifying message

	m_dbc = dbc;
	m_count = msgObject->getCount();
	commonConstruction();

	m_id->setText(QString::number(msgObject->getCanMessage().id, 16));	// ID
	m_len->setValue(msgObject->getCanMessage().data.size());			// DLC
	m_comment->setText(msgObject->getComment());						// Comment

	int i = 0;
	for(;i < msgObject->getCanMessage().data.size(); i++)				// Data
	{
		QString val = QString::number(msgObject->getCanMessage().data.at(i), 16);
		if(val.size() == 1)
			val = "0" + val;

		m_data.at(i)->setText(val);
	}

	m_period->setText(QString::number(msgObject->getPeriod())); // Period
}

void MessageConstructionDialog::commonConstruction()
{
	// Dialog buttons
	QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

	// Message ID
	m_id = new QLineEdit(this);
	m_id->setMaximumWidth(50);
	m_id->setInputMask("\\0\\xHHH");
	m_id->setText("0x000");
	m_id->setToolTip("CAN message ID");
	//m_id->setAlignment(Qt::AlignCenter);
	connect(m_id, SIGNAL(textChanged(QString)), this, SLOT(idChanged()));


	QHBoxLayout *idLayout = new QHBoxLayout();
	//idLayout->addWidget(new QLabel("0x"));
	idLayout->addWidget(m_id);
	idLayout->addSpacing(20);

	// Data length
	m_len = new QSpinBox(this);
	m_len->setMaximum(8);
	m_len->setMaximumWidth(50);
	m_len->setToolTip("CAN message data lenght (DLC)");
	connect(m_len, SIGNAL(valueChanged(int)), this, SLOT(lengthChanged()));

	m_len_label = new QLabel();

	QHBoxLayout *lenLayout = new QHBoxLayout();
	lenLayout->addWidget(m_len);
	lenLayout->addWidget(m_len_label);

	// Period
	m_period = new QLineEdit(this);
	QValidator *period_validator = new QIntValidator(0,10000000, this);
	m_period->setValidator(period_validator);
	m_period->setToolTip("Message transmit period in ms. Use 0 for manual mode.");
	m_period->setMaximumWidth(80);
	m_period->setAlignment(Qt::AlignRight);
	connect(m_period, SIGNAL(textChanged(QString)), this, SLOT(periodChanged(QString)));

	m_period_units_label = new QLabel("ms", this);

	QHBoxLayout *periodLayout = new QHBoxLayout();
	periodLayout->addWidget(m_period);
	periodLayout->addWidget(m_period_units_label);

	// Data
	QHBoxLayout *dataLayout = new QHBoxLayout();

	for(int i = 0; i < 8; i++)
	{
		QLineEdit *e = new QLineEdit(this);
		e->setAlignment(Qt::AlignCenter);
		e->setMaximumWidth(25);
		e->setInputMask("HH");
		e->setText("00");
		m_data.append(e);
		dataLayout->addWidget(e);

		connect(e, SIGNAL(editingFinished()), this, SLOT(updateSignalsList()));
	}

	// Message Name (visible when paired with DBC file)
	m_msg_name = new QLineEdit(this);
	m_msg_name->setToolTip("Message name (from DBC file)");
	m_msg_name->setVisible(m_dbc != 0);
	connect(m_msg_name, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));

	m_msg_name_label = new QLabel("<b>Name</b>");
	m_msg_name_label->setVisible(m_dbc != 0);

	if(m_dbc != 0)
	{
		QCompleter *completer = new QCompleter(m_dbc->getMessageNames(), this);
		m_msg_name->setCompleter(completer);
	}

	// Signal table
	m_signalsView = new QTableWidget(this);
	m_signalsView->setColumnCount(4);
	m_signalsView->setColumnWidth(0, 150);	// Signal name
	m_signalsView->setColumnWidth(1, 100);	// Raw value
	m_signalsView->setColumnWidth(2, 125);	// Value
	m_signalsView->setColumnWidth(3, 80);	// Units
	m_signalsView->setHorizontalHeaderLabels(QStringList() << "Signal name" << "Raw" << "Physical" << "Units");
	m_signalsView->setVisible(m_dbc != 0);
	// Adapt table widget size according to content of table
	m_signalsView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_signalsView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

	m_signals_label = new QLabel("<b>Signals</b>");
	m_signals_label->setVisible(m_dbc != 0);

	m_comment = new QLineEdit(this);

	QHeaderView *verticalHeader = m_signalsView->verticalHeader();
	verticalHeader->sectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(24);

	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow("ID", idLayout);
	formLayout->addRow("Period", periodLayout);
	formLayout->addRow("Length", lenLayout);
	formLayout->addRow("Data", dataLayout);
	formLayout->addRow("Comment", m_comment);
	formLayout->addRow(m_msg_name_label, m_msg_name);
	formLayout->addRow(m_signals_label, m_signalsView);
	formLayout->addWidget(bbox);

	setLayout(formLayout);

	setWindowTitle("Message Construction");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button in title bar

	lengthChanged();
}

void MessageConstructionDialog::accept()
{
	if(m_msgObject != 0)
	{
		// Here we update the parameter that can be changed in the dialog
		m_msgObject->setComment(m_comment->text());
		m_msgObject->setPeriod(m_period->text().toInt());

		CanTraceMessage trcMsg;
		trcMsg.id = m_id->text().toInt(0, 16);
		trcMsg.type = "Tx";
		trcMsg.data = getData();

		m_msgObject->setCanMessage(trcMsg);
	}

	m_msgObject->setTransmit(m_transmitEnable); // Enable transmission if was enabled

	QDialog::accept();
}


void MessageConstructionDialog::reject()
{
	m_msgObject->setTransmit(m_transmitEnable); // Enable transmission if was enabled

	QDialog::reject();
}

void MessageConstructionDialog::lengthChanged()
{
	int i = 0;

	for(; i < m_len->value(); i++)
		m_data.at(i)->setEnabled(true);

	for(; i < 8; i++)
		m_data.at(i)->setEnabled(false);


	if(m_dbc == 0)
		return;

	CanMessage* msg_p = m_dbc->getMessage(m_id->text().toInt(0,16));

	if(msg_p != 0)
	{
		quint8 dbcLen = msg_p->getDataLenght();
		if(m_len->value() != dbcLen)
			m_len_label->setText("<b>Warning </b>: data lenght mismatch with value in DBC (" + QString::number(dbcLen) + ")");
		else
			m_len_label->clear();
	}
	else
		m_len_label->clear();

	updateSignalsList();
}

void MessageConstructionDialog::idChanged()
{
	// ID is valid
	quint32 msgId = m_id->text().toInt(0, 16);

	if (m_dbc != 0)
	{
		// return id + message name
		CanMessage *msg_p = m_dbc->getMessage(msgId);

		if (msg_p != 0)
		{
			m_msg_name->setText(m_dbc->getMessage(msgId)->getName());

			m_len->setValue(msg_p->getDataLenght());

			updateSignalsList();
		}
		else
		{
			m_signalsView->clearContents();
			m_signalsView->setRowCount(0);
			m_msg_name->setText("(Unknown message ID)");
		}
	}
}

void MessageConstructionDialog::updateSignalsList()
{
	if(m_dbc == 0)
		return;


	quint32 msgId = m_id->text().toInt(0, 16); // CAN message ID

	// return id + message name
	CanMessage *msg_p = m_dbc->getMessage(msgId);

	if (msg_p != 0)
	{
		// Update signals values
		QList<SignalValue_t> sig_list = msg_p->getSignalsValue(getData());

		m_signalsView->clearContents();
		m_signalsView->setRowCount(sig_list.count());

		for(int i = 0; i < sig_list.count(); i++)
		{
			SignalValue_t sig = sig_list.at(i);
			QTableWidgetItem *item_name = new QTableWidgetItem(sig.name);
			item_name->setTextAlignment(Qt::AlignVCenter);
			item_name->setFlags(Qt::ItemIsEnabled);
			QTableWidgetItem *item_val = new QTableWidgetItem(sig.valueStr);
			//item_val->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			item_val->setFlags(Qt::ItemIsEnabled); // TODO : make editable
			item_val->setTextAlignment(Qt::AlignCenter);
			QTableWidgetItem *item_unit= new QTableWidgetItem(sig.unit);
			item_unit->setTextAlignment(Qt::AlignCenter);
			item_unit->setFlags(Qt::ItemIsEnabled);
			QTableWidgetItem *item_raw = new QTableWidgetItem(QString::number(sig.rawValue));
			item_raw->setTextAlignment(Qt::AlignCenter);
			//item_raw->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			item_raw->setFlags(Qt::ItemIsEnabled); // TODO : make editable

			m_signalsView->setItem(i, 0, item_name);
			m_signalsView->setItem(i, 1, item_raw);
			m_signalsView->setItem(i, 2, item_val);
			m_signalsView->setItem(i, 3, item_unit);
		}
	}
	else
	{
		m_signalsView->clearContents();
		m_signalsView->setRowCount(0);
	}


	adjustSize();
}

void MessageConstructionDialog::periodChanged(const QString &str)
{
	bool ok;
	int period = str.toInt(&ok);

	if(ok)
	{
		QString units = "ms";

		if(period >= 1000)
			units += "  (";

		if(period >= 3600000) // Hours
			units += QString::number(period/3600000.0, 'f', 2) + "h)";
		else if(period >= 60000) // Minutes
			units += QString::number(period/60000.0, 'f', 2) + "min)";
		else if(period >= 1000 && period < 60000) // Seconds
			units += QString::number(period/1000.0, 'f', 2) + "s)";


		m_period_units_label->setText(units);
	}
	else
	{
		m_period_units_label->setText("ms");
	}

}

void MessageConstructionDialog::nameChanged(const QString &str)
{
	if(m_dbc == 0)
		return;

	CanMessage *msg = m_dbc->getMessage(str);

	if(msg == 0)
		return;

	// Update fields
	m_id->setText("0x" + QString::number(msg->getId(),16));
	m_len->setValue( (msg->getDataLenght() <= 8) ? msg->getDataLenght() : 8 ); // Data length or 8 if value > 8 (shoud not be)

	updateSignalsList();
}

QList<quint8> MessageConstructionDialog::getData()
{
	QList<quint8> data;

	for(int i = 0; i < m_len->value(); i++)
	{
		data.append(m_data.at(i)->text().toInt(0, 16));
	}

	return data;
}
