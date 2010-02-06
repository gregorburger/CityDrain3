#include "simulationthread.h"
#include <simulation.h>
#include <boost/bind.hpp>
#include <boost/signals.hpp>

using namespace boost;
using namespace boost::signals;

SimulationThread::SimulationThread(ISimulation *simulation)
	: simulation(simulation) {
	handler = new TimeStepHandler();
	c = simulation->timestep_after.connect(bind(&TimeStepHandler::after, handler, _1, _2));
}

SimulationThread::~SimulationThread() {
	c.disconnect();
	delete handler;
}

void SimulationThread::run() {
	SimulationParameters sp = simulation->getSimulationParameters();
	simulation->start(sp.start);
}
