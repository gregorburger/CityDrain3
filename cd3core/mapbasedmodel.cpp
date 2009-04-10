#include "mapbasedmodel.h"
#include <iostream>
#include <node.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <simulation.h>
#include <vector>
#include <cd3assert.h>

using namespace boost;

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

Node *MapBasedModel::getNode(const std::string &name) const {
	return names_nodes.find(name)->second;
}

void MapBasedModel::addNode(const std::string &name, Node *node) {
	cd3assert(node, "added node null");
	cd3assert(name != "", "node name empty");
	cd3assert(names_nodes.find(name) == names_nodes.end(), "node name already defined");
	
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

	cd3assert(names_nodes.count(src_node),
			  str(format("source node (%1%) not found") % src_node));
	cd3assert(names_nodes.count(sin_node),
			  str(format("sink node (%1%) not found") % sin_node));

	Node *source = names_nodes.find(src_node)->second;
	Node *sink = names_nodes.find(sin_node)->second;

	sink_nodes.erase(source);
	source_nodes.erase(sink);

	cd3assert(source->const_out_ports->count(src_port),
			  str(format("source node[%1%] port[%2%] not found") % src_node % src_port));

	cd3assert(sink->const_in_ports->count(sin_port),
			  str(format("sink node[%1%] port[%2%] not found") % sin_node % sin_port));

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
virtual name_node_map getNamesAndNodes() const = 0;	std::cout << "number of nodes: " << names_nodes.size() << std::endl;
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


name_node_map MapBasedModel::getNamesAndNodes() const {
	return names_nodes;
}
