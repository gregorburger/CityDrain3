#include "nodeconnection.h"
#include "node.h"

NodeConnection::NodeConnection(Node * source, const std::string &soport,
							   Node *sink, const std::string &siport)
	: source(source), sink(sink), source_port(soport), sink_port(siport) {
}

void NodeConnection::push(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}

void NodeConnection::pull(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}
