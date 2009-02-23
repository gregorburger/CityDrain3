#include "node.h"
#include <flow.h>

void Node::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
}

void Node::deinit() {
}

int Node::getDT() {
	return -1;
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
