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

int Node::ts_f(int time, int dt) {
	int ret;
#pragma omp critical
	ret = this->f(time, dt);
	return ret;
}

void Node::setInPort(const std::string &name, const Flow *inflow) {
	cd3assert(in_ports.find(name) != in_ports.end(),
			  str(format("no such in port (%1%)") % name));
	cd3assert(inflow,
			  str(format("in port (%1%) may not be null") % name));
#ifdef _OPENMP
//#pragma omp critical
#endif
	*in_ports[name] = *inflow;
}

void Node::setOutPort(const std::string &name, const Flow *outflow) {
	cd3assert(out_ports.find(name) != out_ports.end(),
			  str(format("no such out port (%1%)") % name));
	cd3assert(outflow,
			  str(format("out port (%1%) may not be null") % name));
	*out_ports[name] = *outflow;
}

const Flow *Node::getOutPort(const std::string &name) const {
	cd3assert(out_ports.find(name) != out_ports.end(),
			  str(format("no such out port (%1%)") % name));
	return out_ports.find(name)->second;
}

void Node::addInPort(const std::string &name, Flow *inflow) {
	cd3assert(inflow,
			  str(format("adding null in port (%1%)") % name));
	cd3assert(in_ports.find(name) == in_ports.end(),
			  str(format("in port already added (%1%)") % name));
	in_ports[name] = inflow;
}

void Node::addOutPort(const std::string &name, Flow *outflow) {
	cd3assert(outflow,
			  str(format("adding null out port (%1%)") % name));
	cd3assert(out_ports.find(name) == out_ports.end(),
			  str(format("out port already defined (%1%)") % name));
	out_ports[name] = outflow;
}
