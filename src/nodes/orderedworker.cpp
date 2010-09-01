#include "orderedworker.h"
#include <boost/foreach.hpp>
#include <memory>
#include <node.h>
#include <imodel.h>
#include <nodeconnection.h>

OrderedWorker::OrderedWorker(OrderedPipeSimulation *sim,
							 Node *last,
							 ptime _time, int _dt) {
	this->sim = sim;
	this->last = last;
	this->time = _time;
	this->dt = _dt;
	out = std::auto_ptr<tqueue<Node *> >(new tqueue<Node *>());
}

void OrderedWorker::run() {
	sim->timestep_before(sim, time);
	Node *current;
	do {
		current = in->dequeue();
		pullPorts(current);
		current->f(time, dt);
		pushPorts(current);
		out->enqueue(current);
	} while (current != last);
	sim->timestep_after(sim, time);
}

void OrderedWorker::pullPorts(Node *sink) {
	BOOST_FOREACH(NodeConnection *con, sim->getModel()->backwardConnection(sink)) {
		con->pull(dt);
	}
}

void OrderedWorker::pushPorts(Node *source) {
	BOOST_FOREACH(NodeConnection *con, sim->getModel()->forwardConnection(source)) {
		con->push(dt);
	}
}
