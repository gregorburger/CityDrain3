#include "mapbasedmodel.h"
#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <simulation.h>
#include <vector>
#include <cd3assert.h>

typedef std::pair<std::string, Node*> nodes_pair_type;

MapBasedModel::MapBasedModel() {
}

MapBasedModel::~MapBasedModel() {
	BOOST_FOREACH(nodes_pair_type p, names_nodes) {
		p.second->deinit();
		delete p.second;
	}
}

const node_set_type *MapBasedModel::getNodes() const {
	return &all_nodes;
}

void MapBasedModel::addNode(const std::string &name, Node *node) {
	assert(node, "added node null");
	assert(name != "", "node name empty");
	assert(names_nodes.find(name) == names_nodes.end(), "node name already defined");
	
	names_nodes[name] = node;
	all_nodes.insert(node);
	sink_nodes.insert(node);
	source_nodes.insert(node);
//	node->initPorts();
}

void MapBasedModel::addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port) {

	assert(names_nodes.find(src_node) != names_nodes.end(), "source node not found");
	assert(names_nodes.find(sin_node) != names_nodes.end(), "sink node not found");

	Node *source = names_nodes.find(src_node)->second;
	Node *sink = names_nodes.find(sin_node)->second;

	sink_nodes.erase(source);
	source_nodes.erase(sink);

	assert(source->const_out_ports->find(src_port)
		   !=
		   source->const_out_ports->end(), "source node port not found");

	assert(sink->const_in_ports->find(sin_port)
		   !=
		   sink->const_in_ports->end(), "sink node port not found");

	if (fwd_connections.find(source) == fwd_connections.end()) {
		fwd_connections[source] = std::vector<next_node_type>();
	}
	fwd_connections[source].push_back(next_node_type(src_port, sink, sin_port));

	if (bwd_connections.find(sink) == bwd_connections.end()) {
		bwd_connections[sink] = std::vector<next_node_type>();
	}

	bwd_connections[sink].push_back(next_node_type(src_port, source, sin_port));
}

void dumpParameters(Node *n) {
	(void) n;
}

/*void MapBasedModel::dump() {
	std::cout << "number of nodes: " << names_nodes.size() << std::endl;
	node_map::iterator it = names_nodes.begin();
	while (it != names_nodes.end()) {
		std::cout << "node[" << it->first << "]=" << it->second->getNodeName() << std::endl;
		it++;
	}
}*/

void MapBasedModel::initNodes(const SimulationParameters &sp) {
	node_set_type::iterator it = all_nodes.begin();
	while (it != all_nodes.end()) {
		Node *n = *it;
		n->init(sp.start, sp.stop, sp.dt);
		it++;
	}
}

node_set_type MapBasedModel::getSinkNodes() {
	return sink_nodes;
}

node_set_type MapBasedModel::getSourceNodes() {
	return source_nodes;
}

std::vector<next_node_type> MapBasedModel::forward(Node *n) {
	return fwd_connections[n];
}

std::vector<next_node_type> MapBasedModel::backward(Node *n) {
	return bwd_connections[n];
}
