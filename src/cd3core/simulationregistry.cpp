#include "simulationregistry.h"

#include <simulationfactory.h>
#include <simulation.h>
#include <cd3assert.h>

#include <QLibrary>
#include <QDebug>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;

SimulationRegistry::SimulationRegistry() {
}

SimulationRegistry::~SimulationRegistry() {
	typedef std::pair<std::string, const ISimulationFactory *> ttype;
	BOOST_FOREACH(ttype t, registry) {
		delete t.second;
	}
}

void SimulationRegistry::addSimulationFactory(const ISimulationFactory *factory) {
	cd3assert(factory, "factory null");
	registry[factory->getSimulationName()] = factory;
}

void SimulationRegistry::addNativePlugin(const std::string plugin_path) {
	QLibrary l(QString::fromStdString(plugin_path));
	bool loaded = l.load();
	cd3assert(loaded, str(format("could not load plugin %1%: %2%")
						  % plugin_path
						  % l.errorString().toStdString()));
	regSimFunProto regSimFun = (regSimFunProto) l.resolve("registerSimulations");
	if (regSimFun) {
		regSimFun(this);
	} else {
		Logger(Warning) << plugin_path << " has no node register hook";
	}
}

typedef std::pair<std::string, const ISimulationFactory *> snf;
std::vector<std::string> SimulationRegistry::getRegisteredNames() const {
	std::vector<std::string> names;

	BOOST_FOREACH(snf item, registry) {
		names.push_back(item.first);
	}

	return names;
}

bool SimulationRegistry::contains(const std::string &name) const {
	return (registry.find(name) != registry.end());
}

ISimulation *SimulationRegistry::createSimulation(const std::string &name) const {
	cd3assert(contains(name), "no such simulation registered");
	sim_reg_type::const_iterator it = registry.find(name);
	return it->second->createSimulation();
}
