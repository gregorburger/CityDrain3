#include "log.h"
#include <iostream>
#include "node.h"
#include <logger.h>
#include <logsink.h>
#include <ostream>

using namespace std;

Log *Log::instance = 0;

Log *Log::getInstance() {
	if (!instance) {
		cerr << "log not initialed, call Log::init" << endl;
		assert(instance);
	}
	return instance;
}

void Log::init(LogSink *sink, LogLevel max) {
	if (!instance) {
		instance = new Log();
	}
	instance->sink = sink;
	instance->max = max;
}

void Log::shutDown() {
	instance->sink->close();
	delete instance->sink;
	instance->sink = 0;
	delete instance;
}

Log::Log() {
}

Log::~Log() {
	if (instance) {
		//cerr << "Log not deinitialized" << endl;
	}
}

