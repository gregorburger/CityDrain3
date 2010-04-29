#include "logsink.h"

OStreamLogSink::OStreamLogSink(std::ostream &ostream) : out(ostream) {

}

LogSink &OStreamLogSink::operator<<(const std::string &s) {
	out << s;
	return *this;
}

LogSink &OStreamLogSink::operator<<(const char *s) {
	out << s;
	return *this;
}

LogSink &OStreamLogSink::operator<<(int i) {
	out << i;
	return *this;
}

LogSink &OStreamLogSink::operator<<(LSEndl e) {
	out << std::endl;
	return *this;
}

