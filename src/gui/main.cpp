#include <QtGui/QApplication>
#include "mainwindow.h"
#include <log.h>
#include <iostream>

int main(int argc, char *argv[])
{
	Log::init(&cerr, Debug);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
