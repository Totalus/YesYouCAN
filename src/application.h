#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QScopedPointer>

#include "struct_dbc.h"
#include "mainwindow.h"
#include "interface_hwinterface.h"

class Application : public QApplication
{	
	Q_OBJECT

	public:
		static Application *create(int &argc, char **argv, int flags = ApplicationFlags);
		static QList<DbcModel*> &dbcList();
		static QList<HwInterface*> &interfaceList();
		~Application();

	private:
		static QScopedPointer<Application> app; // Instance
		Application(int &argc, char **argv, int flags = ApplicationFlags); // Default constructor

		QList<DbcModel*> m_dbc_list;
		QList<HwInterface*> m_interface_list;
		MainWindow w;
};

#endif // APPLICATION_H
