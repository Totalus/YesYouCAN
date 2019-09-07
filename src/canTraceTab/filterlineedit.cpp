#include "filterlineedit.h"

FilterLineEdit::FilterLineEdit(QWidget *parent) : QLineEdit(parent)
{
	m_patterns	<< "msg.timestamp" << "msg.type" << "msg.id"
				<< "msg.data[" << "msg.len" << "msg.name"
				<< "contains(" << "signal(";

	m_completerModel = new QStringListModel(m_patterns);

	m_completer = new QCompleter(this);
	m_completer->setModel(m_completerModel);
	m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	m_completer->setWidget(this);

	connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void FilterLineEdit::updateCompletionLists(DbcModel *dbc)
{
	if(dbc == 0)
	{
		m_signalNames.clear();
		m_messageNames.clear();
	}
	else
	{
		m_signalNames = dbc->getSignalList();
		m_messageNames = dbc->getMessageNames();
	}
}

void FilterLineEdit::insertCompletion(QString arg)
{
	int index = text().left(cursorPosition()).lastIndexOf(" ");

	if(index < text().left(cursorPosition()).lastIndexOf("("))
		index = text().left(cursorPosition()).lastIndexOf("(");

	setText(text().replace(index + 1, cursorPosition() - index - 1, arg));

}

void FilterLineEdit::keyPressEvent(QKeyEvent *e)
{
	QLineEdit::keyPressEvent(e);

	if(!m_completer)
		return;

	// Get the completion prefix and the need to complete
	bool complete;
	QString prefix = computeCompletionPrefix(&complete);
	m_completer->setCompletionPrefix(prefix);

	QRect cr = cursorRect();
	cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());

	if(complete)
		m_completer->complete(cr);
	else
		m_completer->popup()->hide();

}


QString FilterLineEdit::computeCompletionPrefix(bool *complete)
{
	*complete = true;

	QString line = text(); // Text in line edit
	QString cursor_word; // Word before text cursor
	cursor_word = line.mid(line.left(cursorPosition()).lastIndexOf(" ") + 1,
						   cursorPosition() - line.left(cursorPosition()).lastIndexOf(" ") - 1);

	if(cursor_word.isEmpty())
	{
		*complete = false;
		return cursor_word;
	}
	else if(cursor_word.startsWith("contains("))
	{
		m_completerModel->setStringList(m_signalNames);
		cursor_word.remove(0, QString("contains(").count()); // Get the text after "contains("
		return cursor_word;
	}
	else if(cursor_word.startsWith("signal("))
	{
		m_completerModel->setStringList(m_signalNames);
		cursor_word.remove(0, QString("signal(").count()); // Get the text after "contains("
		return cursor_word;
	}
	else
	{
		m_completerModel->setStringList(m_patterns);
		return cursor_word;
	}

	return "";
}
