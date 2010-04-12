#include "simulationthread.h"
#include <simulation.h>
#include <imodel.h>
#include <boost/bind.hpp>
#include <boost/signals.hpp>

using namespace boost;
using namespace boost::signals;

SimulationThread::SimulationThread() {
	handler = new TimeStepHandler();

}

SimulationThread::~SimulationThread() {
	c.disconnect();
	delete handler;
}

void SimulationThread::run() {
	Q_ASSERT(simulation);
	SimulationParameters sp = simulation->getSimulationParameters();
	c = simulation->timestep_after.connect(bind(&TimeStepHandler::after, handler, _1, _2));
	simulation->getModel()->deinitNodes();
	simulation->getModel()->initNodes(sp);//TODO check for uninited nodes here
	simulation->start(sp.start);
}
