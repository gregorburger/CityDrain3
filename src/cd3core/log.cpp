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

