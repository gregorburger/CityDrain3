#include <Python.h>
#include "pysimulation.h"
#include <simulation.h>
#include <simulationregistry.h>
#include <nodeconnection.h>
#include <node.h>
#include <mapbasedmodel.h>
#include "module.h"

#include <boost/foreach.hpp>
#include <boost/python.hpp>
using namespace boost::python;
using namespace std;

struct PythonCallback {
	PythonCallback(object callable) {
		this->callable = callable;
	}

	void operator()(ISimulation *s, ptime time) {
		try {
			callable(ptr(s), time);
		} catch(...) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			handle_python_exception();
		}
	}
	object callable;
};

static void s_addHandlerAfter(ISimulation &sim, object o) {
	sim.timestep_after.connect(PythonCallback(o));
}

static void s_addHandlerBefore(ISimulation &sim, object o) {
	sim.timestep_before.connect(PythonCallback(o));
}

void wrap_simulation() {
	class_<NodeConnection>("NodeConnection", init<Node *, string, Node *, string>());

	class_<ISimulation, noncopyable>("Simulation", no_init)
		.def("start", &ISimulation::start)
		.def("createConnection", &ISimulation::createConnection, return_value_policy<manage_new_object>())
		.def("setModel", &ISimulation::setModel)
		.def("setSimulationParameters", &ISimulation::setSimulationParameters)
		.def("getSimulationParameters", &ISimulation::getSimulationParameters)
		.def("timestep_after", s_addHandlerAfter)
		.def("timestep_before", s_addHandlerBefore)
		;
	class_<SimulationRegistry>("SimulationRegistry")
		.def("addNativePlugin", &SimulationRegistry::addNativePlugin)
		.def("createSimulation", &SimulationRegistry::createSimulation, return_value_policy<manage_new_object>())
		.def("getRegisteredNames", &SimulationRegistry::getRegisteredNames)
		;
	class_<SimulationParameters>("SimulationParameters", init<string, string, string>())
		.def_readwrite("start", &SimulationParameters::start)
		.def_readwrite("stop", &SimulationParameters::stop)
		.def_readwrite("dt", &SimulationParameters::dt)
		;
}
