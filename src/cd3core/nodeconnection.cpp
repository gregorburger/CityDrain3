#include "nodeconnection.h"
#include "node.h"

NodeConnection::NodeConnection(Node * source, const std::string &soport,
							   Node *sink, const std::string &siport)
	: source(source), sink(sink), source_port(soport), sink_port(siport) {
	cd3assert(source->const_out_ports->count(source_port),
			  str(format("source node[%1%] port[%2%] not found") % source->getId() % source_port));
	cd3assert(sink->const_in_ports->count(sink_port),
			  str(format("sink node[%1%] port[%2%] not found") % sink->getId() % sink_port));

}

void NodeConnection::push(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}

void NodeConnection::pull(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}
