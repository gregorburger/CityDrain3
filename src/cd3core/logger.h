#ifndef LOGGER_H
#define LOGGER_H

#include <cd3globals.h>
#include "log.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
using namespace boost;

using namespace std;

class QString;

class CD3_PUBLIC Logger {
public:
	Logger(LogLevel level = Standard);
	virtual ~Logger();
	Logger &operator<< (LogLevel level);
	Logger &operator<< (const shared_ptr<Node> node);
	Logger &operator<< (const Node *node);
	Logger &operator<< (const char* s);
	Logger &operator<< (const int i);
	Logger &operator<< (const long i);
	Logger &operator<< (const double f);
	Logger &operator<< (const float f);
	Logger &operator<< (const string &i);
	Logger &operator<< (const QString &s);
private:
	string logLevel() const;
	string date() const;
	ostream &out;
	LogLevel level, max;
	bool dirty;
};


#endif // LOGGER_H
