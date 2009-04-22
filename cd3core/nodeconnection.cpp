#include "nodeconnection.h"
#include "node.h"

NodeConnection::NodeConnection(Node * source, const std::string &soport,
							   Node *sink, const std::string &siport)
	: source(source), sink(sink), source_port(soport), sink_port(siport) {
}

void NodeConnection::pull() {
	/*cd3assert(q.empty(), "q empty, can not pull");*/
	Flow f = q.dequeue();
	sink->setInPort(sink_port, &f);
}

void NodeConnection::push() {
	q.enqueue(*source->getOutPort(source_port));
	//q.push();
}
