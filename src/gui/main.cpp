#include <QtGui/QApplication>
#include "mainwindow.h"
#include <flow.h>
#include <log.h>
#include <iostream>

int main(int argc, char *argv[])
{
	Log::init(&cerr, Debug);
	map<string, Flow::CalculationUnit> fd;
	fd["Q"] = Flow::flow;
	Flow::define(fd);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
