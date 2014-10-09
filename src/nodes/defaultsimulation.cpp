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

#include "defaultsimulation.h"
#include <imodel.h>
#include <node.h>
#include <flow.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>

CD3_DECLARE_SIMULATION_NAME(DefaultSimulation)

struct SimPriv {
	node_set_type sources;
	node_set_type done;
};

DefaultSimulation::DefaultSimulation() : ISimulation() {
	sp = new SimPriv();
}

DefaultSimulation::~DefaultSimulation() {
	delete sp;
}

void DefaultSimulation::start(ptime time) {
	sp->sources = model->getSourceNodes();
	ISimulation::start(time);
}

int DefaultSimulation::run(ptime time, int dt) {
	sp->done.clear();
	con_count_type deps = model->getBackwardCounts();

	BOOST_FOREACH(Node *n, sp->sources) {
		run(n, time, deps);

	}
	return dt;
}

void DefaultSimulation::run(Node *n, ptime time, con_count_type &depends) {
	if (sp->done.count(n) > 0) {
		return;
	}
	n->f(time, sim_param.dt);
	sp->done.insert(n);
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		con->push(0);
		depends[con->sink]--;
	}
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		if (depends[con->sink] > 0) {
			continue;
		}
		run(con->sink, time, depends);
	}
	return;
}

void DefaultSimulation::setModel(IModel *model) {
	ISimulation::setModel(model);
}
