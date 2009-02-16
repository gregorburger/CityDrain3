#include "node.h"
#include <flow.h>

std::pair<int, int> Node::MIN_MAX_DT_DONT_CARE = std::make_pair<int, int>(-1, -1);

void Node::setInPort(const std::string &name, const Flow *inflow) {
	*in_ports[name] = *inflow;
}

const Flow *Node::getOutPort(const std::string &name) {
	return out_ports[name];
}

void Node::addInPort(const std::string &name, Flow *inflow) {
	in_ports[name] = inflow;
}

void Node::addOutPort(const std::string &name, Flow *inflow) {
	out_ports[name] = inflow;
}
