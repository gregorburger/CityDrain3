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

#ifndef LOGGER_H
#define LOGGER_H

#include <cd3globals.h>
#include "log.h"

class QString;
class LogSink;

class CD3_PUBLIC Logger {
public:
	Logger(LogLevel level = Standard);
	virtual ~Logger();
	Logger &operator<< (LogLevel level);
	Logger &operator<< (const Node *node);
	Logger &operator<< (const char* s);
	Logger &operator<< (const int i);
	Logger &operator<< (const size_t i);
	Logger &operator<< (const long i);
	Logger &operator<< (const double f);
	Logger &operator<< (const float f);
	Logger &operator<< (const std::string &i);
	Logger &operator<< (const QString &s);
private:
	std::string logLevel() const;
	std::string date() const;
	LogSink &sink;
	LogLevel level, max;
	bool dirty;
};


#endif // LOGGER_H
