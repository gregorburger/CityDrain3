#ifndef TIMESTEPHANDLER_H
#define TIMESTEPHANDLER_H

#include <QObject>
#include <boost/date_time.hpp>

using namespace boost::posix_time;

class ISimulation;

class TimeStepHandler : public QObject
{
Q_OBJECT
public:
	TimeStepHandler();
	virtual ~TimeStepHandler();
	void after(ISimulation *sim, ptime t);

private:
	int old_progress;
Q_SIGNALS:
	void progress(int percent);
	void finished();
};

#endif // TIMESTEPHANDLER_H
