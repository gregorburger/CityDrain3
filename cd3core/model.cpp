#include "model.h"
#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>

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
