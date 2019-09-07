#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include <QtWidgets>
#include <struct_dbc.h>

class FilterLineEdit : public QLineEdit
{
	Q_OBJECT

	public:
		FilterLineEdit(QWidget *parent = nullptr);

		void updateCompletionLists(DbcModel* dbc);

	public slots:
		void insertCompletion(QString arg);

	protected:
		void keyPressEvent(QKeyEvent *e);

	private:
		QCompleter *m_completer;
		QStringListModel *m_completerModel;

		QStringList m_signalNames;
		QStringList m_messageNames;
		QStringList m_patterns;

		QString computeCompletionPrefix(bool* complete);

};

#endif // FILTERLINEEDIT_H
