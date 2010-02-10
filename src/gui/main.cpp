#include <QtGui/QApplication>
#include "mainwindow.h"
#include <log.h>
#include <logsink.h>
#include <iostream>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
