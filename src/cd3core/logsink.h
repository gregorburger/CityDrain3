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

#ifndef LOGSINK_H
#define LOGSINK_H

#include <string>
#include <ostream>

#include <cd3globals.h>
#include "log.h"

class CD3_PUBLIC LSEndl {};

class CD3_PUBLIC LogSink
{
public:
	virtual ~LogSink() {}
	virtual void close() = 0;
	virtual LogSink &operator<<(LogLevel level) = 0;
	virtual LogSink &operator<<(const std::string &string) = 0;
	virtual LogSink &operator<<(const char *string) = 0;
	virtual LogSink &operator<<(int i) = 0;
	virtual LogSink &operator<<(LSEndl) = 0;
};

class CD3_PUBLIC OStreamLogSink : public LogSink {
public:
	OStreamLogSink(std::ostream &ostream);

	void close() {}
	LogSink &operator<<(LogLevel level);
	LogSink &operator<<(const std::string &string);
	LogSink &operator<<(const char *string);
	LogSink &operator<<(int i);
	LogSink &operator<<(LSEndl e);

private:
	std::ostream &out;
};

#endif // LOGSINK_H
