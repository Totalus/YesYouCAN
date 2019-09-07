#ifndef MESSAGECONSTRUCTIONDIALOG_H
#define MESSAGECONSTRUCTIONDIALOG_H


#include <QtWidgets>

#include "struct_dbc.h"
#include "model_cantransmit.h"


class MessageConstructionDialog : public QDialog
{
	Q_OBJECT

	public:
		MessageConstructionDialog(CanTxMessageObject *msgObject, DbcModel *dbc = 0, QWidget *parent = 0, Qt::WindowFlags f = 0);



	public slots:
		void lengthChanged();
		void idChanged();
		void updateSignalsList();
		void nameChanged(const QString& str);
		//void dataChanged();
		void periodChanged(const QString& str);

		void accept();
		void reject();

	private:
		DbcModel *m_dbc;

		// Widgets
		QLineEdit *m_id;
		QSpinBox *m_len;
		QLabel *m_len_label;
		QLineEdit *m_period;
		QLabel *m_period_units_label;
		QList<QLineEdit*> m_data;
		QLineEdit *m_msg_name;
		QLabel *m_msg_name_label;
		QTableWidget *m_signalsView;
		QLabel *m_signals_label;
		QLineEdit *m_comment;
		bool m_transmitEnable;

		CanTxMessageObject *m_msgObject;

		int m_count;

		void commonConstruction();
		QList<quint8> getData();


};

#endif // MESSAGECONSTRUCTIONDIALOG_H
