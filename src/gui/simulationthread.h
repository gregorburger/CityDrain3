#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>
#include "timestephandler.h"
#include <boost/signals.hpp>

class ISimulation;

class SimulationThread : public QThread
{
public:
	SimulationThread(ISimulation *simulation);
	virtual ~SimulationThread();
	void run();
	TimeStepHandler *handler;
	ISimulation *getSimulation() const { return simulation; }
private:
	ISimulation *simulation;
	boost::signals::connection c;
};

#endif // SIMULATIONTHREAD_H
