#include "log.h"
#include <iostream>
#include <node.h>
#include <logger.h>

#include <ostream>
#include <QDebug>

using namespace std;

Log *Log::instance = 0;

Log *Log::getInstance() {
	cd3assert(instance, "log not initialed, call Log::init");
	return instance;
}

void Log::init(ostream *out, LogLevel max) {
	if (!instance) {
		instance = new Log();
	}
	instance->out = out;
	instance->max = max;
}

void Log::shutDown() {
	instance->out->flush();
	if (instance->out != &cout && instance->out != &cerr) {
		delete instance->out;
	}
	instance->out = 0;
}

Log::Log() {
}

Log::~Log() {
	if (instance) {
		cerr << "Log not deinitialized" << endl;
	}
}

