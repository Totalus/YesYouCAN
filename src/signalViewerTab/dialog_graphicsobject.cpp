#include "dialog_graphicsobject.h"
#include "ui_graphicsobjectdialog.h"

GraphicsObjectDialog::GraphicsObjectDialog(DbcModel *dbc, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), ui(new Ui::GraphicsObjectDialog)
{
	ui->setupUi(this);

	m_item = 0;
	m_dbc = dbc;

	// Fill Combo Box
	ui->objectTypeField->insertItem(CustomGraphicsItem::TEXT_ITEM, "Text", CustomGraphicsItem::TEXT_ITEM);
	ui->objectTypeField->insertItem(CustomGraphicsItem::BAR_ITEM, "Bar", CustomGraphicsItem::BAR_ITEM);
	ui->objectTypeField->insertItem(CustomGraphicsItem::COLOR_SHAPE_ITEM, "Color shape", CustomGraphicsItem::COLOR_SHAPE_ITEM);
	//ui->objectTypeField->insertItem(CustomGraphicsItem::INVALID, "Invalid", CustomGraphicsItem::INVALID);

	if(m_dbc != 0)
	{
		QCompleter *completer = new QCompleter(m_dbc->getSignalList(), this);
		ui->signalNameLineEdit->setCompleter(completer);
	}

	connect(ui->objectTypeField, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDialogWidgets()));

	m_scene = new QGraphicsScene(this);
	ui->graphicsPreview->setScene(m_scene);

	ui->rawValueComboBox->addItem("None", 0);
	ui->rawValueComboBox->addItem("Binary", 2);
	ui->rawValueComboBox->addItem("Decimal", 10);
	ui->rawValueComboBox->addItem("Hexadecimal", 16);

	ui->orientationComboBox->addItem("Vertical", 0);
	ui->orientationComboBox->addItem("Horizontal", 1);

	// Update preview when parameter changes
	connect(ui->signalNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui->aliasLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui->showAlias, SIGNAL(stateChanged(int)), this, SLOT(updatePreview()));
	connect(ui->showPhysicalValue, SIGNAL(stateChanged(int)), this, SLOT(updatePreview()));
	connect(ui->rawValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
	connect(ui->showSignalName, SIGNAL(stateChanged(int)), this, SLOT(updatePreview()));
	connect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->angleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->signalValueSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->orientationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));

	connect(ui->colorButton, SIGNAL(clicked(bool)), this, SLOT(pickColor()));
	connect(ui->minColorButton, SIGNAL(clicked(bool)), this, SLOT(pickMinColor()));
	connect(ui->maxColorButton, SIGNAL(clicked(bool)), this, SLOT(pickMaxColor()));

	// Update button color
	//ui->colorButton->setAutoFillBackground(true); // Done in form

	setButtonColor(ui->colorButton, QColor(Qt::blue));
	setButtonColor(ui->minColorButton, QColor(Qt::green));
	setButtonColor(ui->maxColorButton, QColor(Qt::red));

	updateDialogWidgets(); // To create the item in the preview scene
}

GraphicsObjectDialog::~GraphicsObjectDialog()
{
	delete ui;
}

void GraphicsObjectDialog::updateDialogWidgets()
{
	// Hide widgets
	ui->signalNameLabel->hide();
	ui->signalNameLineEdit->hide();
	ui->showSignalName->hide();
	ui->showPhysicalValue->hide();
	ui->rawValueComboBox->hide();
	ui->rawValueLineEdit->hide();
	ui->aliasLabel->hide();
	ui->aliasLineEdit->hide();
	ui->showAlias->hide();
	ui->sizeLabel->hide();
	ui->sizeSpinBox->hide();
	ui->angleLabel->hide();
	ui->angleSpinBox->hide();
	ui->colorButton->hide();
	ui->colorLabel->hide();
	ui->maxValueLabel->hide();
	ui->maxValueSpinbox->hide();
	ui->minValueLabel->hide();
	ui->minValueSpinbox->hide();
	ui->signalValueSlider->hide();
	ui->sizeLabel->setText("Size");
	ui->heighLabel->hide();
	ui->heightSpinBox->hide();
	ui->widthLabel->hide();
	ui->widthSpinBox->hide();
	ui->orientationComboBox->hide();
	ui->orientationLabel->hide();
	ui->maxColorButton->hide();
	ui->minColorButton->hide();
	ui->maxColorLabel->hide();
	ui->minColorLabel->hide();

	// Show different widgets depending on item
	if(ui->objectTypeField->currentData().toInt() == CustomGraphicsItem::TEXT_ITEM)
	{
		ui->aliasLabel->show();
		ui->aliasLineEdit->show();
		ui->showAlias->show();
		ui->signalNameLabel->show();
		ui->signalNameLineEdit->show();
		ui->showSignalName->show();
		ui->showPhysicalValue->show();
		ui->rawValueComboBox->show();
		ui->rawValueLineEdit->show();
		ui->sizeSpinBox->show();
		ui->sizeLabel->show();
		ui->sizeLabel->setText("Text size");

		ui->signalValueSlider->show();

		// Size = font size for text
		ui->sizeSpinBox->setValue(12);

		if(m_item != 0)
		{
			m_scene->removeItem(m_item);
			delete m_item;
			m_item = 0;
		}

		if(m_item == 0)
		{
			m_item = new TextItem(getParameters());
			m_scene->addItem(m_item);
		}
	}
	else if(ui->objectTypeField->currentData().toInt() == CustomGraphicsItem::BAR_ITEM)
	{
		ui->aliasLabel->show();
		ui->aliasLineEdit->show();
		ui->showAlias->show();
		ui->signalNameLabel->show();
		ui->signalNameLineEdit->show();
		ui->showSignalName->show();
		ui->showPhysicalValue->show();
		ui->rawValueComboBox->show();
		ui->rawValueLineEdit->show();
		ui->sizeSpinBox->show();
		ui->sizeLabel->show();
		ui->sizeLabel->setText("Text size");
		ui->maxValueLabel->show();
		ui->maxValueSpinbox->show();
		ui->minValueLabel->show();
		ui->minValueSpinbox->show();
		ui->signalValueSlider->show();
		ui->signalValueSlider->setValue(100);
		ui->heighLabel->show();
		ui->heightSpinBox->show();
		ui->heightSpinBox->setValue(100);
		ui->colorButton->show();
		ui->colorLabel->show();
		ui->widthSpinBox->setValue(20);
		ui->orientationComboBox->show();
		ui->orientationLabel->show();

		// Update button color
		setButtonColor(ui->colorButton, QColor(Qt::blue));
		setButtonColor(ui->minColorButton, QColor(Qt::green));
		setButtonColor(ui->maxColorButton, QColor(Qt::red));

		if(m_item != 0)
		{
			m_scene->removeItem(m_item);
			delete m_item;
			m_item = 0;
		}

		if(m_item == 0)
		{
			m_item = new BarItem(getParameters());
			m_scene->addItem(m_item);
		}
	}
	else if(ui->objectTypeField->currentData().toInt() == CustomGraphicsItem::COLOR_SHAPE_ITEM)
	{
		ui->signalNameLabel->show();
		ui->signalNameLineEdit->show();
		ui->showSignalName->show();
		ui->showPhysicalValue->show();
		ui->rawValueComboBox->show();
		ui->rawValueLineEdit->show();
		ui->aliasLabel->show();
		ui->aliasLineEdit->show();
		ui->showAlias->show();
		ui->sizeLabel->show();
		ui->sizeSpinBox->show();
		ui->signalValueSlider->show();
		ui->widthLabel->show();
		ui->widthSpinBox->show();
		ui->maxColorButton->show();
		ui->minColorButton->show();
		ui->maxColorLabel->show();
		ui->minColorLabel->show();
		ui->sizeLabel->show();
		ui->sizeLabel->setText("Text size");
		ui->sizeSpinBox->show();
		ui->sizeSpinBox->setValue(12);
		ui->widthSpinBox->setValue(80);

		if(m_item != 0)
		{
			m_scene->removeItem(m_item);
			delete m_item;
			m_item = 0;
		}

		if(m_item == 0)
		{
			m_item = new ColorShapeItem(getParameters());
			m_scene->addItem(m_item);
		}
	}

	updatePreview();

}

GraphicsObjectParameters_t GraphicsObjectDialog::getParameters()
{
	GraphicsObjectParameters_t params;

	// Used
	params.itemType = ui->objectTypeField->currentData().toInt();
	params.signalName = ui->signalNameLineEdit->text();
	params.showSignalName = ui->showSignalName->isChecked();
	params.signalAlias = ui->aliasLineEdit->text();
	params.showAlias = ui->showAlias->isChecked();
	params.showPhysicalValue = ui->showPhysicalValue->isChecked();
	params.base = ui->rawValueComboBox->currentData().toInt();
	params.font = QFont("Verdana");
	params.font.setPixelSize(ui->sizeSpinBox->value());
	params.angle = ui->angleSpinBox->value();
	params.color = ui->colorButton->palette().button().color();
	params.minColor = ui->minColorButton->palette().button().color();
	params.maxColor = ui->maxColorButton->palette().button().color();
	params.height = ui->heightSpinBox->value();
	params.width = ui->widthSpinBox->value(); // Fixed value
	params.orientation = ui->orientationComboBox->currentData().toInt();

	// Reserved
	params.showPercent = false;
	params.overrideUnits = false;
	params.min = 0;
	params.max = 0;
	params.gradient = false;
	params.shape = QString();

	return params;
}


void GraphicsObjectDialog::setParameters(GraphicsObjectParameters_t params)
{
	// Set widgets according to parameters
	ui->objectTypeField->setCurrentIndex(params.itemType);
	ui->signalNameLineEdit->setText(params.signalName);
	ui->showSignalName->setChecked(params.showSignalName);
	ui->aliasLineEdit->setText(params.signalAlias);
	ui->showAlias->setChecked(params.showAlias);
	ui->showPhysicalValue->setChecked(params.showPhysicalValue);
	ui->angleSpinBox->setValue(params.angle);
	ui->heightSpinBox->setValue(params.height);
	ui->sizeSpinBox->setValue(params.font.pixelSize());

	setButtonColor(ui->colorButton, params.color);
	setButtonColor(ui->minColorButton, params.minColor);
	setButtonColor(ui->maxColorButton, params.maxColor);

	ui->widthSpinBox->setValue(params.width);

	int index = ui->rawValueComboBox->findData(params.base);
	if(index != -1)
		ui->rawValueComboBox->setCurrentIndex(index);
	else
		ui->rawValueComboBox->setCurrentIndex(0);

	index = ui->orientationComboBox->findData(params.orientation);
	if(index != -1)
		ui->orientationComboBox->setCurrentIndex(index);
	else
		ui->orientationComboBox->setCurrentIndex(0);

	if(params.itemType == CustomGraphicsItem::TEXT_ITEM || params.itemType == CustomGraphicsItem::BAR_ITEM)
		ui->sizeSpinBox->setValue(params.font.pixelSize());
}

void GraphicsObjectDialog::accept()
{
	if(ui->objectTypeField->currentData().toInt() == CustomGraphicsItem::TEXT_ITEM)
	{
		if(ui->signalNameLineEdit->text().isEmpty() && (ui->showSignalName->isChecked() || ui->showPhysicalValue->isChecked() || (ui->rawValueComboBox->currentText() != "None")))
		{
			if(QMessageBox::Yes == QMessageBox::question(this, "No signal name", "Signal name is empty. Disable display of the raw value, physical value and signal name so it becomes a simple text label ?"))
			{
				ui->showAlias->setChecked(true);
				ui->showPhysicalValue->setChecked(false);
				ui->showSignalName->setChecked(false);
				ui->rawValueComboBox->setCurrentText("None");
				return;
			}
			else
			{
				QMessageBox::warning(this, "No signal name", "Signal name can't be empty. Enter a signal name");
				return;
			}

		}

		if(ui->aliasLineEdit->text().isEmpty() && !(ui->showSignalName->isChecked() || ui->showPhysicalValue->isChecked() || (ui->rawValueComboBox->currentText() != "None")))
		{
			QMessageBox::warning(this, "Alias empty", "Enter an alias.");
			return;
		}


		if(!ui->showSignalName->isChecked() && !ui->showAlias->isChecked() && (ui->rawValueComboBox->currentIndex() == 0) && !ui->showPhysicalValue->isChecked())
		{
			QMessageBox::warning(this, "No item displayed", "Nothing to be displayed in this item, check something to show for this signal.");
			return;
		}
	}

	QDialog::accept();
}

void GraphicsObjectDialog::signalNameChanged(QString signalName)
{
	if(m_dbc == 0)
		return;

	CanSignal* sig = m_dbc->getSignal(signalName);

	if(sig == 0)
	{
		// Signal not in DBC file
	}
	else
	{
		// Signal in DBC file
	}
}

// A parameter has changed
void GraphicsObjectDialog::updatePreview()
{
	if(m_item != 0)
	{
		m_item->setParameters(getParameters());

		SignalValue_t sigValue;
		sigValue.rawValue = 0;
		sigValue.valueStr = "0";
		sigValue.unit = QString();
		sigValue.name = ui->signalNameLineEdit->text();
		sigValue.max = 100;
		sigValue.min = 0;
		sigValue.value = 0;

		if(m_dbc != 0)
		{
			CanSignal* sig = m_dbc->getSignal(ui->signalNameLineEdit->text());

			if(sig != 0)
			{
				sigValue.unit = sig->getUnits();
				sigValue.max = sig->getMax();
				sigValue.min = sig->getMin();
				sigValue.value = (sig->getMax() - sig->getMin())*ui->signalValueSlider->value()/100.0 + sig->getMin();
				sigValue.valueStr = QString::number(sigValue.value);
			}
		}
		else
		{
			sigValue.value = ui->signalValueSlider->value();
			sigValue.valueStr = QString::number(sigValue.value);
		}

		m_item->updateSignalValue(sigValue);
	}
}

void GraphicsObjectDialog::pickColor()
{
	colorPicker(ui->colorButton);
}

void GraphicsObjectDialog::pickMinColor()
{
	colorPicker(ui->minColorButton);
}

void GraphicsObjectDialog::pickMaxColor()
{
	colorPicker(ui->maxColorButton);
}

void GraphicsObjectDialog::colorPicker(QPushButton *button)
{
	QColor color = QColorDialog::getColor(button->palette().color(QPalette::Button), this, "Select color");

	setButtonColor(button, color);
}

void GraphicsObjectDialog::setButtonColor(QPushButton *button, QColor color)
{
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::ButtonText, color);
		palette.setColor(QPalette::Button, color);
		button->setPalette(palette);
		button->setText(color.name());
	}

	updatePreview();
}
