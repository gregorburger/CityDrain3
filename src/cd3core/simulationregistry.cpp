/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include <QLibrary> //on top here because clang complains about ambiguous QList::iterator
#include "simulationregistry.h"

#include <simulationfactory.h>
#include <simulation.h>
#include <cd3assert.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

extern "C" void registerSimulations(SimulationRegistry *reg);

SimulationRegistry::SimulationRegistry() {
	Logger(Debug) << "registering builtin Simulations";
	registerSimulations(this);
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

void SimulationRegistry::addNativePlugin(const std::string &plugin_path) {
	QLibrary l(QString::fromStdString(plugin_path));
	bool loaded = l.load();
	if (!loaded && plugin_path == "nodes") {
		Logger(Warning) << "ignoring native plugin \"nodes\" because it is loaded as builtin";
		return;
	}
	if (!loaded) {
		Logger(Error) << str(format("could not load plugin %1%: %2%")
							   % plugin_path
							   % l.errorString().toStdString());
		return;
	}

	regSimFunProto regSimFun = (regSimFunProto) l.resolve("registerSimulations");
	if (regSimFun) {
		regSimFun(this);
	} else {
        Logger(Warning) << plugin_path << " has no simulation register hook";
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
