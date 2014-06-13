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

#include "simulation.h"
#include "modelserializer.h"
#include "imodel.h"
#include "cd3assert.h"
#include "nodeconnection.h"
#include "log.h"
#include "logger.h"
#include "node.h"
#include "icontroller.h"

#include <QTime>
#include <boost/foreach.hpp>

#ifndef PYTHON_DISABLED
#include <boost/filesystem.hpp>
#endif

ISimulation::ISimulation()
 : running(true) {
}

ISimulation::~ISimulation() {

}

void ISimulation::setSimulationParameters(const SimulationParameters &params) {
	sim_param = params;
}

SimulationParameters ISimulation::getSimulationParameters() const {
	return sim_param;
}

void ISimulation::setModel(IModel *m) {
	cd3assert(m, "cannot set null model")
	model = m;
}

IModel *ISimulation::getModel() const {
	return model;
}

void ISimulation::start(ptime time) {
	(void) time;
	cd3assert(model, "model not set");
	if (!model->connected())
		Logger(Warning) << "model not fully connected";
	running = true;
	if (!model->cycleFree()) {
		Logger(Warning) << "model not cyclefree ";
		BOOST_FOREACH(Node *n, model->cycleNodes()) {
			Logger(Warning) << "node:"  << n << "is a cyclenode";
		}
	}
	cd3assert(model->cycleFree(), "use \"cycle_break\" attribute in connection (xml) to split up cycles");
	model->checkModel();

	BOOST_FOREACH(Node *n, *model->getNodes()) {
		n->start();
	}

	QTime ts_before = QTime::currentTime();
	current_time = time + seconds(sim_param.dt);
	int dt;
	while (running && current_time <= sim_param.stop) {
		BOOST_FOREACH(IController *c, controllers) {
			c->before_timestep(this, current_time);
		}

		timestep_before(this, current_time);
		dt = run(current_time, sim_param.dt);
		timestep_after(this, current_time);

		BOOST_FOREACH(IController *c, controllers) {
			c->after_timestep(this, current_time);
		}

		current_time = current_time + seconds(dt);
	}

	BOOST_FOREACH(Node *n, *model->getNodes()) {
		n->stop();
	}

	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	Logger(Standard) << "simulation took:" << duration << "ms";
}

void ISimulation::stop() {
	Logger(Debug) << "interrupted stopping of simulation";
	running = false;
}

void ISimulation::serialize(const std::string &dir) const {
	ModelSerializer ms(model, dir);
	ms.serialize(current_time);
}

void ISimulation::deserialize(const std::string &dir, ptime time) const {
	Logger(Debug) << "deserializing timestep" << to_simple_string(time) << "from" << dir;
	ModelSerializer ms(model, dir);
	ms.deserialize(time);
}

NodeConnection *ISimulation::createConnection(Node * source,
								 const std::string &soport,
								 Node *sink,
								 const std::string &siport) const {
	return new NodeConnection(source, soport, sink, siport);
}

void ISimulation::addController(IController *controller) {
	this->controllers.push_back(controller);
}
