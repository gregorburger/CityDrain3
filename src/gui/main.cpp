#include <QtGui/QApplication>
#include <QtOpenGL>
#include "mainwindow.h"
#include <log.h>
#include <logsink.h>
#include <iostream>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QApplication::setApplicationName("CityDrain3");
	QApplication::setOrganizationName("University of Innsbruck - "
								"Unit of Environmental Engineering");
	QApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
	MainWindow w;
	w.show();
	return a.exec();
}
