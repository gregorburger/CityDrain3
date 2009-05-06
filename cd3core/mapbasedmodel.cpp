#include "mapbasedmodel.h"
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <vector>

#include <node.h>
#include <simulation.h>
#include <cd3assert.h>
#include <nodeconnection.h>

using namespace boost;

typedef pair<string, Node*> nodes_pair_type;

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

Node *MapBasedModel::getNode(const string &name) const {
	return names_nodes.find(name)->second;
}

void MapBasedModel::addNode(const string &name, Node *node) {
	cd3assert(node, "added node null");
	cd3assert(name != "", "node name empty");
	cd3assert(names_nodes.find(name) == names_nodes.end(), "node name already defined");
	
	names_nodes[name] = node;
	all_nodes.insert(node);
	sink_nodes.insert(node);
	source_nodes.insert(node);
	uncon_nodes.insert(node);
	fwd_connections[node] = vector<NodeConnection *>();
	bwd_connections[node] = vector<NodeConnection *>();
//	node->initPorts();
}

void MapBasedModel::addConnection(const string &src_node,
					   const string &src_port,
					   const string &sin_node,
					   const string &sin_port) {

	cd3assert(names_nodes.count(src_node),
			  str(format("source node (%1%) not found") % src_node));
	cd3assert(names_nodes.count(sin_node),
			  str(format("sink node (%1%) not found") % sin_node));

	Node *source = names_nodes.find(src_node)->second;
	Node *sink = names_nodes.find(sin_node)->second;

	sink_nodes.erase(source);
	source_nodes.erase(sink);
	uncon_nodes.erase(source);
	uncon_nodes.erase(sink);

	cd3assert(source->const_out_ports->count(src_port),
			  str(format("source node[%1%] port[%2%] not found") % src_node % src_port));

	cd3assert(sink->const_in_ports->count(sin_port),
			  str(format("sink node[%1%] port[%2%] not found") % sin_node % sin_port));

	NodeConnection *connection = new NodeConnection(source, src_port, sink, sin_port);
	bwd_connections[sink].push_back(connection);
	fwd_connections[source].push_back(connection);
}

void dumpParameters(Node *n) {
	(void) n;
}

/*void MapBasedModel::dump() {
virtual name_node_map getNamesAndNodes() const = 0;	cout << "number of nodes: " << names_nodes.size() << endl;
	node_map::iterator it = names_nodes.begin();
	while (it != names_nodes.end()) {
		cout << "node[" << it->first << "]=" << it->second->getNodeName() << endl;
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

vector<next_node_type> MapBasedModel::forward(Node *n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");

	vector<next_node_type> fwd;
	BOOST_FOREACH(NodeConnection *c, fwd_connections[n]) {
		fwd.push_back(next_node_type(c->source_port, c->sink, c->sink_port));
	}
	return fwd;
}

vector<next_node_type> MapBasedModel::backward(Node *n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");

	vector<next_node_type> bwd;
	BOOST_FOREACH(NodeConnection *c, bwd_connections[n]) {
		bwd.push_back(next_node_type(c->source_port, c->source, c->sink_port));
	}
	return bwd;
}

vector<NodeConnection *> MapBasedModel::forwardConnection(Node *n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no forward connection for node");
	return fwd_connections[n];
}

vector<NodeConnection *> MapBasedModel::backwardConnection(Node *n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no backward connection for node");
	return bwd_connections[n];
}

name_node_map MapBasedModel::getNamesAndNodes() const {
	return names_nodes;
}

bool MapBasedModel::connected() const {
	BOOST_FOREACH(Node * n, uncon_nodes) {
		std::cerr << "node " << getNodeName(n) << "not connected" << std::endl;
	}
	return uncon_nodes.empty();
}

string MapBasedModel::getNodeName(const Node *node) const {
//	cd3assert(all_nodes.count(node), "node not in model");
	pair<string, Node *> pn;
	BOOST_FOREACH(pn, names_nodes) {
		if (pn.second == node) {
			return pn.first;
		}
	}
	cd3assert(false, "node not in model");
	return "";
}

void MapBasedModel::checkModel() const {
	BOOST_FOREACH(Node *node, all_nodes) {
		size_t iports = node->const_in_ports->size();
		size_t bcons = bwd_connections.at(node).size();
		size_t fcons = fwd_connections.at(node).size();
		size_t oports = node->const_out_ports->size();
		string name = getNodeName(node);
		cd3assert(iports == bcons, str(format("node %1% has %2% in ports but only %3% connections") % name % iports % bcons));
		cd3assert(oports == fcons, str(format("node %1% has %2% out ports but only %3% connections") % name % oports % fcons));
	}
}


con_count_type MapBasedModel::getBackwardCounts() const {
	con_count_type counts;
	BOOST_FOREACH(Node *node, all_nodes) {
		counts[node] = bwd_connections.at(node).size();
	}

	return counts;
}

con_count_type MapBasedModel::getForwardCounts() const {
	con_count_type counts;
	BOOST_FOREACH(Node *node, all_nodes) {
		counts[node] = fwd_connections.at(node).size();
	}

	return counts;
}
