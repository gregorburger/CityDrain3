/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
