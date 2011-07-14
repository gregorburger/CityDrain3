#include "simulationthread.h"
#include <simulation.h>
#include <imodel.h>
#include <boost/bind.hpp>
#include <boost/signals.hpp>
#include <pythonexception.h>
#include <logger.h>

using namespace boost;
using namespace boost::signals;

SimulationThread::SimulationThread()
	: handler(new TimeStepHandler()), simulation(0), failed(false) {
}

SimulationThread::~SimulationThread() {
	c.disconnect();
	delete handler;
}

void SimulationThread::run() {
	Q_ASSERT(simulation);
	failed = false;
	SimulationParameters sp = simulation->getSimulationParameters();
	c = simulation->timestep_after.connect(bind(&TimeStepHandler::after, handler, _1, _2));
	try {
		simulation->getModel()->deinitNodes();
		simulation->getModel()->initNodes(sp);//TODO check for uninited nodes here
		simulation->start(sp.start + seconds(sp.dt));
	} catch (PythonException e) {
		exception = e;
		failed = true;
	}
}
