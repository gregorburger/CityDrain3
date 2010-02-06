#include "log.h"
#include <iostream>
#include "node.h"
#include <logger.h>

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
	delete instance;
}

Log::Log() {
}

Log::~Log() {
	if (instance) {
		//cerr << "Log not deinitialized" << endl;
	}
}

