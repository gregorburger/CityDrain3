#ifndef LOGSINK_H
#define LOGSINK_H

#include <string>
#include <ostream>

class LSEndl {};

class LogSink
{
public:
	virtual ~LogSink() {}
	virtual void close() = 0;
	virtual LogSink &operator<<(const std::string &string) = 0;
	virtual LogSink &operator<<(const char *string) = 0;
	virtual LogSink &operator<<(int i) = 0;
	virtual LogSink &operator<<(LSEndl) = 0;
};

class OStreamLogSink : public LogSink {
public:
	OStreamLogSink(std::ostream &ostream);

	void close() {}
	LogSink &operator<<(const std::string &string);
	LogSink &operator<<(const char *string);
	LogSink &operator<<(int i);
	LogSink &operator<<(LSEndl e);

private:
	std::ostream &out;
};

#endif // LOGSINK_H
