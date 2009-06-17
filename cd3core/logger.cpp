#include "logger.h"
#include <node.h>
#include <QDateTime>

Logger::Logger(LogLevel level)
 : out(*Log::getInstance()->out), level(level) {
	dirty = false;
	this->max = Log::getInstance()->max;
	if (level >= max) {
		out << logLevel() << " " << date() << "|";
		dirty = true;
	}
}

Logger::~Logger() {
	if (dirty)
		out << endl;
}

Logger &Logger::operator <<(LogLevel new_level) {
	level = new_level;
	out << "\n" << logLevel() << " " << date() << "|";
	dirty = true;
	return *this;
}

Logger &Logger::operator <<(const Node *n) {
	if (level < max) {
		return *this;
	}
	out << " Node[" << n->getId() << "] ";
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const char* s) {
	if (level < max) {
		return *this;
	}
	out << " " << s;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const int i) {
	if (level < max) {
		return *this;
	}
	out << " " << i;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const long i) {
	if (level < max) {
		return *this;
	}
	out << " " << i;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const double f) {
	if (level < max) {
		return *this;
	}
	out << " " << f;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const float f) {
	if (level < max) {
		return *this;
	}
	out << " " << f;
	dirty = true;
	return *this;
}

Logger &Logger::operator<< (const string &s) {
	if (level < max) {
		return *this;
	}
	out << " " << s;
	dirty = true;
	return *this;
}


string Logger::logLevel() const {
	switch (level) {
		case Debug:
			return "DEBUG";
		case Warn:
			return "WARN";
		case Standard:
			return "STANDARD";
		case Error:
			return "ERROR";
	}
}

string Logger::date() const {
	return QDateTime::currentDateTime().toString().toStdString();
}
