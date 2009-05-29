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
		std::string src_port, snk_port;
		Node *next;
		next = con->sink;
		src_port = con->source_port;
		snk_port = con->sink_port;

		next->setInPort(snk_port, n->getOutPort(src_port));

		depends[next]--;

		if (depends[next] > 0) {
			return;
		}
		run(next, time, depends);
	}
	return;
}

void DefaultSimulation::setModel(IModel *model) {
	ISimulation::setModel(model);
	sources = model->getSourceNodes();
}
