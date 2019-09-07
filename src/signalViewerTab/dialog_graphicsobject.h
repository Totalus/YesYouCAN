#ifndef GRAPHICSOBJECTDIALOG_H
#define GRAPHICSOBJECTDIALOG_H

#include <QtWidgets>
#include "customgraphicsitem.h"
#include "struct_dbc.h"

// Items
#include "textitem.h"
#include "baritem.h"
#include "colorshapeitem.h"


namespace Ui {
	class GraphicsObjectDialog;
}

class GraphicsObjectDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit GraphicsObjectDialog(DbcModel *dbc = 0, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowCloseButtonHint);
		~GraphicsObjectDialog();

		GraphicsObjectParameters_t getParameters(); // Return parameters according to filled widgets
		void setParameters(GraphicsObjectParameters_t params); // Fill widgets according to parameters

	public slots:
		void updateDialogWidgets();	// Update the displayed widget when the item type changes
		void signalNameChanged(QString); // Update some parameters when the signal name changes and a DBC file is specified
		void updatePreview(); // Update the preview

		void pickColor(); // Execute color picker dialog and changes the color setting
		void pickMinColor();
		void pickMaxColor();

		void accept();

	private:
		void colorPicker(QPushButton* button);
		void setButtonColor(QPushButton* button, QColor color);

	private:
		Ui::GraphicsObjectDialog *ui;

		DbcModel *m_dbc;
		CustomGraphicsItem *m_item;
		QGraphicsScene *m_scene;
};



/*
class WatcherObjectDialog : public QDialog
{
	Q_OBJECT

	public:
		WatcherObjectDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void itemTypeChanged();

	private:
		QComboBox *m_objectType;
		QFormLayout *m_paramLayout;

		QLineEdit *m_sigName;
		QLineEdit *m_sigAlias;
		QCheckBox *m_useAlias;
		QCheckBox *m_showRawValue;
		QCheckBox *m_showValue;
};

*/

#endif // GRAPHICSOBJECTDIALOG_H
