#include "mapbasedmodel.h"

#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>
#include <cassert>
#include <simulation.h>



typedef std::pair<std::string, Node*> nodes_pair_type;

MapBasedModel::MapBasedModel() {
}

MapBasedModel::~MapBasedModel() {
	BOOST_FOREACH(nodes_pair_type p, names_nodes) {
		delete p.second;
	}
}


void MapBasedModel::addNode(const std::string &name, Node *node) {
	assert(node);
	assert(name != "");
	assert(names_nodes.find(name) == names_nodes.end());
	
	names_nodes[name] = node;
	all_nodes.insert(node);
	sink_nodes.insert(node);
	source_nodes.insert(node);
	node->initPorts();
}

void MapBasedModel::addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port) {

	assert(names_nodes.find(src_node) != names_nodes.end());
	assert(names_nodes.find(sin_node) != names_nodes.end());

	Node *source = names_nodes.find(src_node)->second;
	Node *sink = names_nodes.find(sin_node)->second;

	sink_nodes.erase(source);
	source_nodes.erase(sink);

	assert(source->const_out_ports->find(src_port) != source->const_out_ports->end());
	assert(sink->const_in_ports->find(sin_port) != sink->const_in_ports->end());

	fwd_connections[end_point_type(source, src_port)] = end_point_type(sink, sin_port);
}

void dumpParameters(Node *n) {
	(void) n;
}

void MapBasedModel::dump() {
	std::cout << "number of nodes: " << names_nodes.size() << std::endl;
	node_map::iterator it = names_nodes.begin();
	while (it != names_nodes.end()) {
		std::cout << "node[" << it->first << "]=" << it->second->getNodeName() << std::endl;
		it++;
	}
}

void MapBasedModel::initNodes(const SimulationParameters &sp) {
	BOOST_FOREACH(nodes_pair_type p, names_nodes) {
		p.second->init(sp.start, sp.stop, sp.dt);
	}
}

std::set<Node *> MapBasedModel::sinkNodes() {
	return sink_nodes;
}

std::set<Node *> MapBasedModel::sourceNodes() {
	return source_nodes;
}
