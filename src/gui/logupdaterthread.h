#ifndef LOGUPDATERTHREAD_H
#define LOGUPDATERTHREAD_H

#include <QThread>
#include <QObject>
#include <QString>

class LogUpdaterThread : public QThread {
	Q_OBJECT
public:
	LogUpdaterThread();

Q_SIGNALS:
	void newLogLine(const QString &line);

private:
	void run();

	std::stringstream *ss;
	bool running;
};

#endif // LOGUPDATERTHREAD_H
