#include "defaultsimulation.h"
#include <model.h>
#include <node.h>
#include <flow.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <map>

CD3_DECLARE_SIMULATION_NAME(DefaultSimulation)

DefaultSimulation::DefaultSimulation() {
}

DefaultSimulation::~DefaultSimulation() {
}

int DefaultSimulation::run(int time, int dt) {
	con_count_type deps = model->getBackwardCounts();
	BOOST_FOREACH(Node *n, sources) {
		run(n, time, deps);
	}
	return dt;
}

void DefaultSimulation::run(Node *n, int time, con_count_type &depends) {
	n->f(time, sim_param.dt);
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		con->push(0);
		depends[con->sink]--;
		if (depends[con->sink] > 0) {
			return;
		}
		run(con->sink, time, depends);
	}
	return;
}

void DefaultSimulation::setModel(IModel *model) {
	ISimulation::setModel(model);
	sources = model->getSourceNodes();
}
