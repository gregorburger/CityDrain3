#include "logsink.h"

OStreamLogSink::OStreamLogSink(std::ostream &ostream) : out(ostream) {

}

LogSink &OStreamLogSink::operator<<(const std::string &string) {
	out << string;
	return *this;
}

LogSink &OStreamLogSink::operator<<(const char *string) {
	out << string;
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

