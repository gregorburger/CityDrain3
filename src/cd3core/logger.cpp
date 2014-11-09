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

#include "logger.h"
#include "node.h"
#include <QDateTime>
#include <QString>
#include <logsink.h>

Logger::Logger(LogLevel level)
 : sink(*Log::getInstance()->sink), level(level) {
	dirty = false;
	this->max = Log::getInstance()->max;
	if (level >= max) {
		sink << level << logLevel() << " " << date() << "|";
		dirty = true;
	}
}

Logger::~Logger() {
	if (dirty)
		sink << LSEndl();
}

Logger &Logger::operator <<(LogLevel new_level) {
	level = new_level;
	sink << "\n" << level << logLevel() << " " << date() << "|";
	dirty = true;
	return *this;
}

Logger &Logger::operator <<(const Node *n) {
	if (level < max) {
		return *this;
	}
	sink << " Node[" << n->getId() << "] ";
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const char* s) {
	if (level < max) {
		return *this;
	}
	sink << " " << s;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const int i) {
	if (level < max) {
		return *this;
	}
	sink << " " << i;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const size_t i) {
	if (level < max) {
		return *this;
	}
	sink << " " << i;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const long i) {
	if (level < max) {
		return *this;
	}
	sink << " " << i;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const double f) {
	if (level < max) {
		return *this;
	}
	sink << " " << f;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const float f) {
	if (level < max) {
		return *this;
	}
	sink << " " << f;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const std::string &s) {
	if (level < max) {
		return *this;
	}
	sink << " " << s;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const QString &s) {
	if (level < max) {
		return *this;
	}
	sink << " " << s.toStdString();
	dirty = true;
	return *this;
}


std::string Logger::logLevel() const {
	switch (level) {
		case Debug:
			return "DEBUG";
		case Warning:
			return "WARN";
		case Standard:
			return "STANDARD";
		case Error:
			return "ERROR";
	}
	return "UNKNOWN";
}

std::string Logger::date() const {
	return QDateTime::currentDateTime().toString().toStdString();
}
