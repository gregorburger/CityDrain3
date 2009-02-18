#include "simulation.h"

#include <model.h>
#include <node.h>
#include <boost/foreach.hpp>
#include <iostream>

Simulation::Simulation() {
}

void Simulation::setSimulationParameters(const SimulationParameters &param) {
	sp = param;
}

void Simulation::start(IModel *model) {
	model->initNodes(sp);

	node_set_type sources = model->sourceNodes();

	for (int time = sp.start; time <= sp.stop; time += sp.dt) {
		BOOST_FOREACH(Node *n, sources) {
			run(model, n, time);
			std::cout << time << std::endl;
		}
	}
}

//typedef std::vector<next_node_type> vector_next_node_type;

void Simulation::run(IModel *model, Node *n, int time) {
	n->f(time, sp.dt);

	BOOST_FOREACH(next_node_type con, model->forward(n)) {
		std::string src_port, snk_port;
		Node *next;
		boost::tuples::tie(src_port, next, snk_port) = con;

		next->setInPort(snk_port, n->getOutPort(src_port));
		run(model, next, time);
	}
}
