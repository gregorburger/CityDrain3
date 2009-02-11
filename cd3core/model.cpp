#include "model.h"
#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>
#include <cassert>

Model::Model() {
}

Model::~Model() {
	std::pair<std::string, Node*> p;
	BOOST_FOREACH(p, nodes) {
		delete p.second;
	}
}


void Model::addNode(const std::string &name, Node *node) {
	assert(node);
	assert(name != "");
	assert(nodes.find(name) == nodes.end());
	nodes[name] = node;
}

void Model::addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port) {

	assert(nodes.find(src_node) != nodes.end());
	assert(nodes.find(sin_node) != nodes.end());

	Node *source = nodes.find(src_node)->second;
	Node *sink = nodes.find(sin_node)->second;

	assert(source->const_out_ports->find(src_port) != source->const_out_ports->end());
	assert(sink->const_in_ports->find(sin_port) != sink->const_in_ports->end());

#warning TODO implement connections
}

void dumpParameters(Node *n) {
	(void) n;
}

void Model::dump() {
	std::cout << "number of nodes: " << nodes.size() << std::endl;
	node_map::iterator it = nodes.begin();
	while (it != nodes.end()) {
		std::cout << "node[" << it->first << "]=" << it->second->getNodeName() << std::endl;
		it++;
	}
}
