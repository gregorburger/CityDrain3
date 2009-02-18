#include "mapbasedmodel.h"

#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>
#include <cassert>
#include <simulation.h>
#include <vector>

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

	if (fwd_connections.find(source) == fwd_connections.end()) {
		fwd_connections[source] = std::vector<next_node_type>();
	}
	fwd_connections[source].push_back(next_node_type(src_port, sink, sin_port));
	//fwd_connections[source]. = end_point_type(sink, sin_port);

	if (bwd_connections.find(sink) == bwd_connections.end()) {
		bwd_connections[sink] = std::vector<next_node_type>();
	}

	//bwd_connections[sink][sin_port] = end_point_type(source, src_port);
	bwd_connections[sink].push_back(next_node_type(sin_port, source, src_port));
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
	node_set_type::iterator it = all_nodes.begin();
	while (it != all_nodes.end()) {
		Node *n = *it;
		//std::cout << "initing node type " << n->getNodeName();
		n->init(sp.start, sp.stop, sp.dt);
		//std::cout << " done" << std::endl;
		it++;
	}
}

node_set_type MapBasedModel::sinkNodes() {
	return sink_nodes;
}

node_set_type MapBasedModel::sourceNodes() {
	return source_nodes;
}

std::vector<next_node_type> MapBasedModel::forward(Node *n) {
	return fwd_connections[n];
}

std::vector<next_node_type> MapBasedModel::backward(Node *n) {
	return bwd_connections[n];
}
