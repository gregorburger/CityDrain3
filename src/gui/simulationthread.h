#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>
#include "timestephandler.h"
#include <boost/signals.hpp>
#include <pythonexception.h>

class ISimulation;

class SimulationThread : public QThread
{
public:
	SimulationThread();
	virtual ~SimulationThread();
	void setSimulation(ISimulation *simulation) { this->simulation = simulation; }
	void run();
	TimeStepHandler *handler;
	ISimulation *getSimulation() const { return simulation; }
	inline bool hasFailed() const { return failed; }
	PythonException exception;
private:
	boost::signals::connection c;
	ISimulation *simulation;
	bool failed;
};

#endif // SIMULATIONTHREAD_H
