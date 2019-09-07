#ifndef TRANSMITWIDGET_H
#define TRANSMITWIDGET_H

#include <QtWidgets>
#include <QStringList>

#include "struct_dbc.h"
#include "model_canmessageviewer.h"
#include "pannel_canmsgdetails.h"
#include "dialog_dbcdialog.h"
#include "widget_abstracttab.h"
#include "dialog_itemdialog.h"
#include "interface_hwinterface.h"
#include "interface_wcan.h"
#include "dialog_wcandialog.h"
#include "dialog_exportdialog.h"
#include "model_cantransmit.h"
#include "dialog_messageconstruction.h"
#include "view_customtransmittable.h"


class Document;

class CanTransmitterWidget : public AbstractTabWidget
{
	Q_OBJECT

	public:
		CanTransmitterWidget(QWidget *parent, Document *document);
		~CanTransmitterWidget();

		void setDbc(DbcModel *dbc);

		// From AbstractTabWidget
		virtual QMenu* specificMenu();

	public slots:
		//void selectionChangedSlot(QModelIndex);

		void onRightClick(const QPoint &point);
		void newMessage();
		void editMessage(const QModelIndex &index);
		void sendMessageManual(QModelIndexList indexes);

		void sendMessageToInterface(CanTraceMessage &msg);

		void setDbc();

		void setDevice();

		void updateStatusBar();

	private:
		void createActions();
		void populateMenu();
		void populateToolBar();
		void createStatusBar();


	private:
		DbcModel *m_dbc;
		CustomTransmitTableView *m_view;
		CanTransmitterModel *m_model;
		CanMsgWidget *m_msgWidget;
		HwInterface *m_device;

		// Menu related
		QMenu *m_traceMenu;
		QAction* m_associateDbcAct;
		QAction* m_deviceAct;
		//QAction* m_saveAct;

		// Status bar related
		QStatusBar* m_statusBar;
		QLabel*		m_statusLabel;

		QPushButton *m_status_nb_msg;
		QPushButton *m_status_dbc;
		QPushButton *m_status_device;

		// Tool bar related
		QToolBar* m_toolbar;

};

#endif // TRANSMITWIDGET_H
