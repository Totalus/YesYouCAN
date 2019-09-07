
#include "application.h"

int main(int argc, char *argv[])
{
	Application *a = Application::create(argc, argv);

	return a->exec();
}
