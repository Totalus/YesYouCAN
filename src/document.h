#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "widget_abstracttab.h"
#include "widget_cantrace.h"
#include "widget_dbctree.h"

//class AbstractTabWidget;

enum DOC_TYPE_t
{
	ROOT_DOCUMENT = 1000,		// Used as a container for other document (like a folder)
	CAN_TRACE = 1001,
	CAN_DBC = 1002,
	CAN_FIXED_TRACE = 1003,
	CAN_INTERFACE = 1004,		// Not a document type, but this definition is needed
	CAN_TRANSMIT = 1005,
	CAN_WATCHER = 1006,
	CAN_GRAPH = 1007
};

enum FILE_TYPE_t
{
	CAN_TRACE_BUSMASTER,
	CAN_TRACE_PEAK,
	CAN_DATABASE,
	UNKNOWN_FILE_TYPE
};


class Document : public QObject
{
	Q_OBJECT

	public:
		Document(DOC_TYPE_t type, QString filePath = QString() , QList<DbcModel*> *dbc_list = 0, QWidget* parent = 0);
		~Document();

		// Action functions
		//bool open(); // Open document (create view widget to display)
		//void create(); // Create document (create empty view widget)

		// Getters
		bool isOpened();
		bool isSaved();
		bool canBeSaved();
		bool exists();
		QString fileName();
		QString filePath();
		QString documentName();
		AbstractTabWidget* getViewWidget();
		HwInterface* getHwInterface();
		DOC_TYPE_t getType();
		FILE_TYPE_t getFileType();

		// Parent / Children function
		bool hasChildren();
		bool hasParent();
		void addChildren(Document *doc);
		QList<Document*> getChildren();
		Document* getParent();
		Document* getChildren(QString filePath); // Returns the Document object with the same path if contained in children
		void removeChild(Document *doc);
		QList<Document*> getAllChildrenRecursive();	// Returns the list of all children document recursively
		QList<Document*> getAllRecursive();	// Returns the list of this document and all children document recursively
		QIcon getIcon();
		// "Row" functions for use by TreeModel
		Document* getChildren(int row); // Returns the Documents object in its children list, returns 0 if invalid index
		int row();	// Returns the row of this item in the parent item

		// Setters
		void setOpened(bool opened);
		void setSaved(bool saved);
		void setFilePath(QString filePath);
		void setViewWidget(AbstractTabWidget* widget);
		void setHwInterface(HwInterface* iface);
		void setExists(bool exists);
		void setDocumentName(QString name);

		//virtual void saveDocument(); // Function to save the document

	signals:
		void documentChanged();

	private:
		void setParent(Document *doc);

	private:
		QString m_fileName;				// Document file name
		QString m_filePath;				// Full document path
		QString m_documentName;			// Document name
		DOC_TYPE_t m_type;				// Document type

		bool m_saved;					// Document is saved (without new modification)
		bool m_opened;					// Document is opened (in view widget)
		bool m_exists;					// Document exists
		AbstractTabWidget *m_widget;	// Pointer to the widget displaying the document (if document is opened)
		HwInterface *m_interface;		// Pointer to the related hardware interface

		QList<DbcModel*> *m_dbc_list;	// Link to dbc list from MainWindow (needed by some view widgets)

		QList<Document*> m_children_doc;// Children documents
		Document* m_parent_doc;			// Parent document
};

#endif // DOCUMENT_H
