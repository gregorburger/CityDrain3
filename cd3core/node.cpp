#include "node.h"
#include <flow.h>
#include <simulation.h>

void Node::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
}

void Node::deinit() {
}

int Node::getDT(const SimulationParameters &p) const {
	return dt == -1 ? p.dt : dt;
}

void Node::setDT(int dt) {
	this->dt = dt;
}

void Node::setInPort(const std::string &name, const Flow *inflow) {
	*in_ports[name] = *inflow;
}

const Flow *Node::getOutPort(const std::string &name) {
	return out_ports[name];
}

void Node::addInPort(const std::string &name, Flow *inflow) {
	assert(inflow);
	in_ports[name] = inflow;
}

void Node::addOutPort(const std::string &name, Flow *outflow) {
	assert(outflow);
	out_ports[name] = outflow;
}
