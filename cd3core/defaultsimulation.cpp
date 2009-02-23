#include "defaultsimulation.h"
#include <model.h>
#include <node.h>
#include <boost/foreach.hpp>
#include <iostream>


DefaultSimulation::DefaultSimulation() {
}

DefaultSimulation::~DefaultSimulation() {
}

void DefaultSimulation::addController(IController *c) {
	controller.push_back(c);
}

void DefaultSimulation::setSimulationParameters(const SimulationParameters &param) {
	sp = param;
}

SimulationParameters DefaultSimulation::getSimulationParameters() const {
	return sp;
}

void DefaultSimulation::start(IModel *model) {
	//model->initNodes(sp);

	node_set_type sources = model->sourceNodes();

	for (int time = sp.start; time <= sp.stop; time += sp.dt) {
		std::map<Node *, int> deps = getDepends(model);
		BOOST_FOREACH(Node *n, sources) {
			run(model, n, time, deps);
		}
		std::cout << time << std::endl;
	}
}

//typedef std::vector<next_node_type> vector_next_node_type;

void DefaultSimulation::run(IModel *model, Node *n, int time, std::map<Node *, int> &depends) {
	n->f(time, sp.dt);

	BOOST_FOREACH(next_node_type con, model->forward(n)) {
		std::string src_port, snk_port;
		Node *next;
		boost::tuples::tie(src_port, next, snk_port) = con;

		next->setInPort(snk_port, n->getOutPort(src_port));

		depends[next]--;

		if (depends[next] > 0) {
			return;
		}
		run(model, next, time, depends);
	}
}

void DefaultSimulation::suspend() {
}

std::map<Node *, int> DefaultSimulation::getDepends(IModel *model) {
	std::map<Node *, int> deps;
	//constnode_set_type *nodes = model->getNodes();
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		deps[node] = model->backward(node).size();
	}

	return deps;
}
