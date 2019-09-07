#include "document.h"

Document::Document(DOC_TYPE_t type, QString filePath, QList<DbcModel*> *dbc_list, QWidget* parent) : QObject(parent)
{
	m_type = type;
	setFilePath(filePath);

	m_opened = false;
	m_exists = QFile::exists(filePath);
	m_saved = m_exists;
	m_widget = 0;			// View widget (Abstract Tab Widget children class)

	m_dbc_list = dbc_list;	// Link to dbc list from MainWindow (needed by some view widgets)
	m_parent_doc = 0;
	m_interface = 0;
}

Document::~Document()
{
	if(hasChildren())
	{

		while(hasChildren()) // for(int i = 0; i < m_children_doc.count(); i++) // delete removes children from list
		{
			delete m_children_doc.at(0);
		}
	}

	if(m_widget != 0)
	{
		delete m_widget;
		m_widget = 0;
	}

	if(hasParent())
	{
		m_parent_doc->removeChild(this);
	}
}

QString Document::fileName()
{
	return m_fileName;
}

QString Document::filePath()
{
	return m_filePath;
}

QString Document::documentName()
{
	if(m_documentName.isEmpty())
		return m_fileName;
	else
		return m_documentName;
}

bool Document::isOpened()
{
	return m_opened;
}

bool Document::isSaved()
{
	return m_saved;
}

bool Document::exists()
{
	return 	QFile::exists(m_filePath);
}

bool Document::canBeSaved()
{
	if(m_type == CAN_FIXED_TRACE)
		return false;
	else
		return true;
}


AbstractTabWidget* Document::getViewWidget()
{
	return m_widget;
}

HwInterface* Document::getHwInterface()
{
	return m_interface;
}

DOC_TYPE_t Document::getType()
{
	return m_type;
}

void Document::setOpened(bool opened)
{
	m_opened = opened;
}

void Document::setSaved(bool saved)
{
	m_saved = saved;
}

void Document::setDocumentName(QString name)
{
	m_documentName = name;
}

void Document::setFilePath(QString filePath)
{
	m_filePath = filePath;
	m_fileName = filePath.split('\\').last().split('/').last(); // File name only
	emit documentChanged();
}

void Document::setViewWidget(AbstractTabWidget* widget)
{
	m_widget = widget;
}

FILE_TYPE_t Document::getFileType()
{
	QString extension = m_filePath.split('.').last();

	if(extension == "dbc")
		return CAN_DATABASE;

	if(extension == "trc")
		return CAN_TRACE_PEAK;

	if(extension == "log")
		return CAN_TRACE_BUSMASTER;

	return UNKNOWN_FILE_TYPE;
}

void Document::setExists(bool exists)
{
	m_exists = exists;
}


bool Document::hasChildren()
{
	return !m_children_doc.empty();
}

bool Document::hasParent()
{
	if(m_parent_doc != 0)
		return true;
	else
		return false;
}

void Document::addChildren(Document *doc)
{
	// Add children
	if(doc == 0)
		return;

	if(m_children_doc.contains(doc))
		return; // Already children

	m_children_doc.append(doc); // Add to children list
	doc->setParent(this);		// Set its parent as this document

	emit documentChanged();
}

void Document::setParent(Document *doc)
{
	if(hasParent())
		m_parent_doc->removeChild(this);

	m_parent_doc = doc;

	emit documentChanged();
}

QList<Document*> Document::getChildren()
{
	return m_children_doc;
}

Document* Document::getParent()
{
	return m_parent_doc;
}

void Document::removeChild(Document *doc)
{
	m_children_doc.removeOne(doc);

	emit documentChanged();
}

QList<Document*> Document::getAllRecursive()
{
	QList<Document*> list;

	for(int i = 0; i < m_children_doc.count(); i++)
	{
		list.append(m_children_doc.at(i)->getAllRecursive());
	}

	list.append(this); // Add current document
	return list;
}

QList<Document*> Document::getAllChildrenRecursive()
{
	QList<Document*> list;

	for(int i = 0; i < m_children_doc.count(); i++)
	{
		list.append(m_children_doc.at(i)->getAllRecursive());
	}

	return list;
}

void Document::setHwInterface(HwInterface *iface)
{
	m_interface = iface;
}

QIcon Document::getIcon()
{
	switch(m_type)
	{
		case CAN_TRACE:
			return QIcon(":/icons/trace");

		case CAN_DBC:
			return QIcon(":/icons/dbc");

		case CAN_FIXED_TRACE:
			return QIcon(":/icons/receive");

		case CAN_INTERFACE:
			return QIcon(":/icons/interface");

		case CAN_TRANSMIT:
			return QIcon(":/icons/transmit");

		case CAN_WATCHER:
			return QIcon(":/icons/eye");

		default:
			return QIcon();
	}
}

Document* Document::getChildren(QString filePath)
{
	// Check if already part of loaded / opened documents
	QList<Document*> document_list = getAllChildrenRecursive();

	for(int j = 0; j < document_list.count(); j++)
	{
		if(document_list.at(j)->filePath() == filePath) // File path match
		{
			return document_list.at(j); // Return document object for that children
		}
	}

	return 0; // Children not found
}

Document* Document::getChildren(int row)
{
	if(row >= m_children_doc.count() || row < 0)
		return 0; // Invalid index
	else
		return m_children_doc.at(row);
}

int Document::row()
{
	if(m_parent_doc)
		return m_parent_doc->m_children_doc.indexOf(const_cast<Document*>(this));
	else
		return 0;
}
