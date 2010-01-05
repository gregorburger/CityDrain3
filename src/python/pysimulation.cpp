#include "pysimulation.h"
#include <simulation.h>
#include <simulationregistry.h>
#include <nodeconnection.h>
#include <node.h>
#include <mapbasedmodel.h>

#include <boost/foreach.hpp>
#include <boost/python.hpp>
using namespace boost::python;
using namespace std;

struct PythonCallback {
	PythonCallback(object callable) {
		this->callable = callable;
	}

	void operator()(ISimulation *s, int time) {
		try {
			callable(ptr(s), time);
		} catch(python::error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
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

static python::list sr_getRegisteredNames(SimulationRegistry &nr) {
	python::list names;
	BOOST_FOREACH(string name, nr.getRegisteredNames()) {
		names.append(name);
	}
	return names;
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
		.def("getRegisteredNames", sr_getRegisteredNames)
		;
	class_<SimulationParameters>("SimulationParameters", init<int, int, int>())
		.def_readwrite("start", &SimulationParameters::start)
		.def_readwrite("stop", &SimulationParameters::stop)
		.def_readwrite("dt", &SimulationParameters::dt)
		;
}
