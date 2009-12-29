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

struct ISimulationWrapper : ISimulation, wrapper<ISimulation> {
	ISimulationWrapper(PyObject *_self) : self(_self) {
		Py_INCREF(self);
	}

	~ISimulationWrapper() {
		Py_DECREF(self);
	}
	int run(int time, int dt) {
		return this->get_override("run")(time, dt);
	}
	PyObject *self;
};

static python::list sr_getRegisteredNames(SimulationRegistry &nr) {
	python::list names;
	BOOST_FOREACH(string name, nr.getRegisteredNames()) {
		names.append(name);
	}
	return names;
}

static NodeConnection *sim_createConnection(ISimulation &sim, Node &src, string src_p, Node &snk, string snk_p) {
	return sim.createConnection(&src, src_p, &snk, snk_p);
}

static void sim_setModel(ISimulation &sim, MapBasedModel &model) {
	sim.setModel(&model);
}

void wrap_simulation() {
	class_<NodeConnection>("NodeConnection", init<Node *, string, Node *, string>());
	class_<ISimulation, auto_ptr<ISimulationWrapper>, boost::noncopyable>("Simulation")
		.def("start", &ISimulationWrapper::start)
		.def("run", &ISimulationWrapper::run)
		.def("createConnection", sim_createConnection, return_value_policy<manage_new_object>())
		.def("setModel", sim_setModel)
		.def("setSimParams", &ISimulation::setSimulationParameters)
		;
	implicitly_convertible<auto_ptr<ISimulationWrapper>, auto_ptr<ISimulation> >();
	class_<SimulationRegistry>("SimulationRegistry")
		.def("addNativePlugin", &SimulationRegistry::addNativePlugin)
		.def("createSimulation", &SimulationRegistry::createSimulation, return_value_policy<manage_new_object>())
		.def("getRegisteredNames", sr_getRegisteredNames)
		;
	class_<SimulationParameters>("SimulationParameters", init<int, int, int>());
}
