#include "simulationregistry.h"

#include <simulationfactory.h>
#include <simulation.h>
#include <cd3assert.h>
#include <boost/foreach.hpp>

SimulationRegistry::SimulationRegistry() {
}

SimulationRegistry::~SimulationRegistry() {
	typedef std::pair<std::string, const ISimulationFactory *> ttype;
	BOOST_FOREACH(ttype t, registry) {
		delete t.second;
	}
}

void SimulationRegistry::addSimulationFactory(const ISimulationFactory *factory) {
	assert(factory, "factory null");
	registry[factory->getSimulationName()] = factory;
}

bool SimulationRegistry::contains(const std::string &name) const {
	return (registry.find(name) != registry.end());
}

ISimulation *SimulationRegistry::createSimulation(const std::string &name) const {
	assert(contains(name), "no such simulation registered");
	sim_reg_type::const_iterator it = registry.find(name);
	return it->second->createSimulation();
}
