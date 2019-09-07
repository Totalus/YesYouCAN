#include "application.h"

QScopedPointer<Application> Application::app;

Application::Application(int &argc, char **argv, int flags) : QApplication(argc, argv, flags)
{
	w.show();
}

Application::~Application()
{
	for(int i = 0; i < dbcList().length(); i++)
	{
		delete dbcList().at(i);
	}

	for(int i = 0; i < interfaceList().count(); i++)
	{
		interfaceList().at(i)->disconnect();
		delete interfaceList().at(i);
	}
}

Application* Application::create(int &argc, char **argv, int flags)
{
	if(app.isNull())
		app.reset(new Application(argc, argv, flags));

	return app.data();
}

QList<DbcModel*>& Application::dbcList()
{
	return app.data()->m_dbc_list;
}

QList<HwInterface*>& Application::interfaceList()
{
	return app.data()->m_interface_list;
}
