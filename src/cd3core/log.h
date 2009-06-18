#ifndef LOG_H
#define LOG_H

#include <cd3globals.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

using namespace std;

class Node;
class Simulation;
class Logger;

enum LogLevel {
	Debug = 0,
	Standard = 1,
	Warning = 2,
	Error = 3
};

#define LOG_HEAD \
	if (level <= max) return *this;\
	out << logLevel() << ": " << "\n"

class CD3_PUBLIC Log
{
public:
	static void init(ostream *out = &cout, LogLevel max = Standard);
	static void shutDown();
	static Log *getInstance();
	friend class Logger;
private:
	Log();
	virtual ~Log();

	static Log *instance;

	ostream *out;
	LogLevel max;
};

#endif // LOG_H
