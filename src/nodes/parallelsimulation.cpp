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

#include "parallelsimulation.h"
#include <imodel.h>
#include <node.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>
#ifdef _OPENMP
#include <omp.h>
#endif

CD3_DECLARE_SIMULATION_NAME(ParallelSimulation)

ParallelSimulation::ParallelSimulation() {
#ifdef _OPENMP
	Logger(Standard) << "number of threads: " << omp_get_num_threads() << " of " <<  omp_get_max_threads();
#endif
}

ParallelSimulation::~ParallelSimulation() {
}

int ParallelSimulation::run(ptime time, int dt) {
	setNodeDepends();
	int sources_size = sources.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
#endif
	for (int i = 0; i < sources_size; i++) {
		Node *n = sources[i];
		run(n, time);
	}

	return dt;
}


void ParallelSimulation::run(Node *n, ptime time) {
	n->f(time, sim_param.dt);
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		con->push(sim_param.dt);
		Node *next = con->sink;

//#pragma omp atomic
		next->num_inputed--;

		cd3assert(next->num_inputed >= 0, "race condition");
		if (next->num_inputed > 0) {
			return;
		}

		run(next, time);
	}
}

void ParallelSimulation::setModel(IModel *model) {
	cd3assert(model, "model null");
	ISimulation::setModel(model);

	node_set_type set_sources = model->getSourceNodes();
	sources = std::vector<Node *>(set_sources.begin(), set_sources.end());
}

void ParallelSimulation::setNodeDepends() const {
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		node->num_inputed = model->backwardConnection(node).size();
	}
}
