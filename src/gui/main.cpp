#include <QtGui/QApplication>
#include <QFileInfo>
#include <QDir>
#include "mainwindow.h"
#include <log.h>
#include <logsink.h>
#include <iostream>
#include <noderegistry.h>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QApplication::setApplicationName("CityDrain3");
	QApplication::setOrganizationName("University of Innsbruck - "
								"Unit of Environmental Engineering");
	QApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
#ifndef PYTHON_DISABLED
	QFileInfo me(argv[0]);
	NodeRegistry::addToPythonPath(me.dir().path().toStdString());
#endif
	MainWindow w;
	w.show();

	return a.exec();
}
