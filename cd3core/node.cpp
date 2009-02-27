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
	assert(in_ports.find(name) != in_ports.end());
	assert(inflow);
	*in_ports[name] = *inflow;
}

const Flow *Node::getOutPort(const std::string &name) const {
	assert(out_ports.find(name) != out_ports.end());
	return out_ports.find(name)->second;
}

void Node::addInPort(const std::string &name, Flow *inflow) {
	assert(inflow);
	assert(in_ports.find(name) == in_ports.end());
	in_ports[name] = inflow;
}

void Node::addOutPort(const std::string &name, Flow *outflow) {
	assert(outflow);
	assert(out_ports.find(name) == out_ports.end());
	out_ports[name] = outflow;
}
