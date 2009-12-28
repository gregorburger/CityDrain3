#include "pysimulation.h"
#include <simulation.h>
#include <simulationregistry.h>
#include <nodeconnection.h>
#include <node.h>

#include <boost/foreach.hpp>
#include <boost/python.hpp>
using namespace boost::python;
using namespace std;

struct SimulationWrapper : ISimulation, wrapper<ISimulation> {
	int run(int time, int dt) {
		return this->get_override("run")(time, dt);
	}
};

static python::list sr_getRegisteredNames(SimulationRegistry &nr) {
	python::list names;
	BOOST_FOREACH(string name, nr.getRegisteredNames()) {
		names.append(name);
	}
	return names;
}

void wrap_simulation() {
        class_<NodeConnection>("NodeConnection", init<Node *, string, Node *, string>());
	class_<SimulationWrapper, boost::noncopyable>("Simulation")
		.def("start", &ISimulation::start)
		.def("run", pure_virtual(&ISimulation::run))
		.def("createConnection", &ISimulation::createConnection, return_value_policy<manage_new_object>())
		;
	class_<SimulationRegistry>("SimulationRegistry")
		.def("addNativePlugin", &SimulationRegistry::addNativePlugin)
		.def("createSimulation", &SimulationRegistry::createSimulation, return_value_policy<manage_new_object>())
		.def("getRegisteredNames", sr_getRegisteredNames)
		;
}
