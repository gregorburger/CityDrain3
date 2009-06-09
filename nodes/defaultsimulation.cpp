#include "defaultsimulation.h"
#include <model.h>
#include <node.h>
#include <flow.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>
#include <iostream>

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

int DefaultSimulation::run(int time, int dt) {
	sp->done.clear();
	con_count_type deps = model->getBackwardCounts();

	BOOST_FOREACH(Node *n, sp->sources) {
		run(n, time, deps);

	}
	return dt;
}

void DefaultSimulation::run(Node *n, int time, con_count_type &depends) {
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
			return;
		}
		run(con->sink, time, depends);
	}
	return;
}

void DefaultSimulation::setModel(IModel *model) {
	ISimulation::setModel(model);
	cd3assert(model->connected(), "model not fully connected");
	cd3assert(model->cycleFree(), "model not cyclefree");
	model->checkModel();
	sp->sources = model->getSourceNodes();
}
