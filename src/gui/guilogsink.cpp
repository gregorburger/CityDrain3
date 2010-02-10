#include "guilogsink.h"
#include <QPlainTextEdit>
#include <sstream>
#include <log.h>

GuiLogSink::GuiLogSink() {
}

GuiLogSink::~GuiLogSink() {
}

LogSink &GuiLogSink::operator<<(const std::string &string) {
	buf += QString::fromStdString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(const char *string) {
	buf += QString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(int i) {
	buf += QString("%0").arg(i);
	return *this;
}

LogSink &GuiLogSink::operator<<(LSEndl i) {
	Q_EMIT newLogLine(buf);
	buf.clear();
	return *this;
}
