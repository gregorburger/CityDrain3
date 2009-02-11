#include "model.h"
#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>

Model::Model()
{
}

Model::~Model() {
	std::pair<std::string, boost::shared_ptr<Node> > p;
	BOOST_FOREACH(p, nodes) {
		p.second.reset();
	}
}


void Model::addNode(const std::string &name, boost::shared_ptr<Node> node) {
	nodes[name] = node;
}

void dumpParameters(Node *n) {

}

void Model::dump() {
	std::cout << "number of nodes: " << nodes.size() << std::endl;
	node_map::iterator it = nodes.begin();
	while (it != nodes.end()) {
		std::cout << "node[" << it->first << "]=" << it->second->getNodeName() << std::endl;
		it++;
	}
}
