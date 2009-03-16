#include "defaultsimulation.h"
#include <model.h>
#include <node.h>
#include <flow.h>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <map>
#include <cd3assert.h>

CD3_DECLARE_SIMULATION_NAME(DefaultSimulation)

struct SimPriv {
	IModel *model;
	SimulationParameters sp;
	std::vector<IController *> controller;
	int runDT;
};

DefaultSimulation::DefaultSimulation() {
	sp = new SimPriv();
}

DefaultSimulation::~DefaultSimulation() {
	delete sp;
}

void DefaultSimulation::addController(IController *c) {
	sp->controller.push_back(c);
}

void DefaultSimulation::setSimulationParameters(const SimulationParameters &param) {
	sp->sp = param;
}

SimulationParameters DefaultSimulation::getSimulationParameters() const {
	return sp->sp;
}

void DefaultSimulation::start(IModel *model) {
	sp->model = model;
	node_set_type sources = sp->model->getSourceNodes();

	for (int time = sp->sp.start; time <= sp->sp.stop; time += sp->sp.dt) {
		std::map<Node *, int> deps = createDependsMap();
		BOOST_FOREACH(Node *n, sources) {
			run(n, time, deps);
		}
		std::cout << time << std::endl;
	}
}

//typedef std::vector<next_node_type> vector_next_node_type;

void DefaultSimulation::run(Node *n, int time, std::map<Node *, int> &depends) {
	n->f(time, sp->sp.dt);
	BOOST_FOREACH(next_node_type con, sp->model->forward(n)) {
		std::string src_port, snk_port;
		Node *next;
		boost::tuples::tie(src_port, next, snk_port) = con;

		next->setInPort(snk_port, n->getOutPort(src_port));

		depends[next]--;

		if (depends[next] > 0) {
			return;
		}
		run(next, time, depends);
	}
	return;
}

void DefaultSimulation::suspend() {
}

std::map<Node *, int> DefaultSimulation::createDependsMap() const {
	std::map<Node *, int> deps;
	//constnode_set_type *nodes = model->getNodes();
	BOOST_FOREACH(Node *node, *sp->model->getNodes()) {
		deps[node] = sp->model->backward(node).size();
	}

	return deps;
}

int DefaultSimulation::getMaxDt() const {
	int dt = sp->sp.dt;
	BOOST_FOREACH(Node *node, *sp->model->getNodes()) {
		dt = std::max(dt, node->getDT(sp->sp));
	}
	return dt;
}
