#ifndef LOGGER_H
#define LOGGER_H

#include <cd3globals.h>
#include "log.h"

#include <boost/shared_ptr.hpp>
using namespace boost;

class QString;
class LogSink;

class CD3_PUBLIC Logger {
public:
	Logger(LogLevel level = Standard);
	virtual ~Logger();
	Logger &operator<< (LogLevel level);
	Logger &operator<< (const shared_ptr<Node> node);
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
