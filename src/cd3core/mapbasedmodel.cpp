#include "mapbasedmodel.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <vector>

#include "node.h"
#include <simulation.h>
#include <cd3assert.h>
#include <nodeconnection.h>

using namespace boost;

typedef pair<string, shared_ptr<Node> > nodes_pair_type;

MapBasedModel::MapBasedModel() {
}

MapBasedModel::~MapBasedModel() {
	BOOST_FOREACH(nodes_pair_type p, names_nodes) {
		p.second->deinit();
//		delete p.second;
	}
	BOOST_FOREACH(NodeConnection *con, all_connections) {
		delete con;
	}
}

const node_set_type *MapBasedModel::getNodes() const {
	return &all_nodes;
}

shared_ptr<Node> MapBasedModel::getNode(const string &name) const {
	cd3assert(names_nodes.count(name), str(format("no node with such name (%1%) registered") % name));
	return names_nodes.find(name)->second;
}

void MapBasedModel::addNode(shared_ptr<Node> node) {
	cd3assert(node->getId() != "", "node has no id");
	cd3assert(node, "cannot add null node");
	cd3assert(names_nodes.find(node->getId()) == names_nodes.end(), "node name already defined");

	names_nodes[node->getId()] = node;
	all_nodes.insert(node);
	sink_nodes.insert(node);
	source_nodes.insert(node);
	uncon_nodes.insert(node);
	fwd_connections[node] = vector<NodeConnection *>();
	bwd_connections[node] = vector<NodeConnection *>();
}

void MapBasedModel::addConnection(NodeConnection *con) {
	cd3assert(con, "connection must no be null");
	shared_ptr<Node> source = con->source;
	shared_ptr<Node> sink = con->sink;

	sink_nodes.erase(source);
	source_nodes.erase(sink);
	uncon_nodes.erase(source);
	uncon_nodes.erase(sink);

	bwd_connections[sink].push_back(con);
	fwd_connections[source].push_back(con);
	all_connections.insert(con);
}

void MapBasedModel::initNodes(const SimulationParameters &sp) {
	node_set_type::iterator it = all_nodes.begin();
	while (it != all_nodes.end()) {
		shared_ptr<Node> n = *it;
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

vector<NodeConnection *> MapBasedModel::forwardConnection(shared_ptr<Node> n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no forward connection for node");
	return fwd_connections[n];
}

const vector<NodeConnection *> MapBasedModel::forwardConnection(shared_ptr<Node> n) const {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no forward connection for node");
	return fwd_connections.at(n);
}

vector<NodeConnection *> MapBasedModel::backwardConnection(shared_ptr<Node> n) {
	cd3assert(n, "Node null");
	cd3assert(all_nodes.count(n), "node not in model");
	cd3assert(fwd_connections.count(n), "no backward connection for node");
	return bwd_connections[n];
}

name_node_map MapBasedModel::getNamesAndNodes() const {
	return names_nodes;
}

bool MapBasedModel::connected() const {
	BOOST_FOREACH(shared_ptr<Node>  n, uncon_nodes) {
		std::cerr << "node " << n->getId() << "not connected" << std::endl;
	}
	return uncon_nodes.empty();
}

typedef pair<string, Flow*> port_type;
void MapBasedModel::checkModel() const {
	BOOST_FOREACH(shared_ptr<Node> node, all_nodes) {
		BOOST_FOREACH(port_type port, *node->const_in_ports) {
			bool found = false;
			BOOST_FOREACH(NodeConnection *con, bwd_connections.at(node)) {
				if (con->sink_port == port.first) {
					found = true;
					break;
				}
			}
			if (!found) {
				cerr << "WARNING: inport " << port.first << " of node " << node->getId() <<  " is not connected" << endl;
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
				cerr << "WARNING: outport " << port.first << " of node " << node->getId() <<  " is not connected" << endl;
			}
		}
	}
}


con_count_type MapBasedModel::getBackwardCounts() const {
	con_count_type counts;
	BOOST_FOREACH(shared_ptr<Node> node, all_nodes) {
		counts[node] = bwd_connections.at(node).size();
	}

	return counts;
}

con_count_type MapBasedModel::getForwardCounts() const {
	con_count_type counts;
	BOOST_FOREACH(shared_ptr<Node> node, all_nodes) {
		counts[node] = fwd_connections.at(node).size();
	}

	return counts;
}

node_set_type MapBasedModel::cycleNodes() const {
	node_set_type cycle_nodes;

	BOOST_FOREACH(shared_ptr<Node> n, all_nodes) {
		node_set_type reachable;
		if (cycleNodesHelper(n, reachable)) {
			cycle_nodes.insert(n);
		}
	}

	return cycle_nodes;
}

bool MapBasedModel::cycleNodesHelper(shared_ptr<Node> n,
									 node_set_type reachable) const {
	bool cycle = false;
	reachable.insert(n);
	BOOST_FOREACH(NodeConnection *con, fwd_connections.at(n)) {
		if (reachable.count(con->sink))
			cycle = true;
		else
			cycle = cycle || cycleNodesHelper(con->sink, reachable);
	}
	return cycle;
}

bool MapBasedModel::cycleFree() const {
	return cycleNodes().empty();
}
