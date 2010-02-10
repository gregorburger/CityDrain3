#include "logupdaterthread.h"
#include <QPlainTextEdit>
#include <sstream>
#include <log.h>

LogUpdaterThread::LogUpdaterThread() : running(true) {
	ss = new std::stringstream();
	Log::init(ss, Debug);
}

void LogUpdaterThread::run() {
	char line[1024];
	while (running) {
		ss->getline(line, 1024);
		cout << "line" << endl;
		Q_EMIT newLogLine(QString(line));
	}
}
