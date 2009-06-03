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

const node_set_type MapBasedModel::getSourceNodes() const {
	return source_nodes;
}

vector<NodeConnection *> MapBasedModel::forwardConnection(Node *n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no forward connection for node");
	return fwd_connections[n];
}

const vector<NodeConnection *> MapBasedModel::forwardConnection(Node *n) const {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no forward connection for node");
	return fwd_connections.at(n);
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
typedef pair<string, Flow*> port_type;
void MapBasedModel::checkModel() const {
	BOOST_FOREACH(Node *node, all_nodes) {
		BOOST_FOREACH(port_type port, *node->const_in_ports) {
			bool found = false;
			BOOST_FOREACH(NodeConnection *con, bwd_connections.at(node)) {
				if (con->sink_port == port.first) {
					found = true;
					break;
				}
			}
			if (!found) {
				cerr << "WARNING: inport " << port.first << " of node " << getNodeName(node) <<  " is not connected" << endl;
			}
		}

		BOOST_FOREACH(port_type port, *node->const_out_ports) {
			bool found = false;
			BOOST_FOREACH(NodeConnection *con, fwd_connections.at(node)) {
				if (con->source_port == port.first) {
					found = true;
					break;
				}
			}
			if (!found) {
				cerr << "WARNING: outport " << port.first << " of node " << getNodeName(node) <<  " is not connected" << endl;
			}
		}
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

node_set_type MapBasedModel::cycleNodes() const {
	node_set_type cycle_nodes;

	BOOST_FOREACH(Node *n, all_nodes) {
		const vector<NodeConnection*> fwds = fwd_connections.at(n);
		if (cycleNodesHelper(n, fwds)) {
			cycle_nodes.insert(n);
		}
	}

	return cycle_nodes;
}

bool MapBasedModel::cycleNodesHelper(Node *n,
									 const vector<NodeConnection *> &fwds) const {
	BOOST_FOREACH(NodeConnection *con, fwds) {
		if (con->sink == n)
			return true;
		else
			return cycleNodesHelper(n, fwd_connections.at(con->sink));
	}
	return false;
}

bool MapBasedModel::cycleFree() const {
	return cycleNodes().empty();
}
