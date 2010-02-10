#ifndef GUILOGSINK_H
#define GUILOGSINK_H

#include <logsink.h>
#include <QObject>
#include <QString>

class GuiLogSink : public QObject, public LogSink {
	Q_OBJECT
public:
	GuiLogSink();
	virtual ~GuiLogSink();

	void close() {};

	LogSink &operator<<(const std::string &string);
	LogSink &operator<<(const char *string);
	LogSink &operator<<(int i);
	LogSink &operator<<(LSEndl i);

Q_SIGNALS:
	void newLogLine(const QString &line);
private:
	QString buf;
};

#endif // GUILOGSINK_H
