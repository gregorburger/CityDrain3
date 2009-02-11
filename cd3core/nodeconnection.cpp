#include "nodeconnection.h"
#include "node.h"

NodeConnection::NodeConnection(const std::string &soport, Node * source, const std::string &siport, Node *sink)
	: source(soport, source), sink(siport, sink) {
}

const endpoint &NodeConnection::getSink() {
	return source;
}

const endpoint &NodeConnection::getSoure() {
	return sink;
}
