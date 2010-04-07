#include "mapbasedmodel.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <vector>

#include "node.h"
#include <simulation.h>
#include <cd3assert.h>
#include <nodeconnection.h>

using namespace boost;

typedef pair<string, Node *> nodes_pair_type;

MapBasedModel::MapBasedModel() {
}

MapBasedModel::~MapBasedModel() {
	BOOST_FOREACH(nodes_pair_type p, names_nodes) {
		p.second->deinit();
		delete p.second;
	}
	BOOST_FOREACH(NodeConnection *con, all_connections) {
		delete con;
	}
}

const node_set_type *MapBasedModel::getNodes() const {
	return &all_nodes;
}

const con_set_type *MapBasedModel::getConnections() const {
	return &all_connections;
}

Node *MapBasedModel::getNode(const string &name) const {
	cd3assert(names_nodes.count(name), str(format("no node with such name (%1%) registered") % name));
	return names_nodes.find(name)->second;
}

void MapBasedModel::addNode(const string &id, Node *node) {
	cd3assert(id != "", "node has no id");
	cd3assert(node, "cannot add null node");
	cd3assert(names_nodes.find(id) == names_nodes.end(), "node name already defined");

	node->setId(id);
	names_nodes[node->getId()] = node;
	all_nodes.insert(node);
	sink_nodes.insert(node);
	source_nodes.insert(node);
	uncon_nodes.insert(node);
	fwd_connections[node] = vector<NodeConnection *>();
	bwd_connections[node] = vector<NodeConnection *>();
}

void MapBasedModel::removeNode(Node *node) {
	cd3assert(node->getId() != "", "node has no id");
	cd3assert(node, "cannot remove null node");
	cd3assert(all_nodes.count(node), "no such node in model");
	Logger(Debug) << "removing node " << node->getClassName()<< "[" << node->getId() << "]";
	names_nodes.erase(node->getId());
	all_nodes.erase(node);
	if (sink_nodes.count(node)) {
		Logger(Debug) << "removing node " << node->getId() << "from sink nodes";
		sink_nodes.erase(node);
	}
	if (source_nodes.count(node)) {
		Logger(Debug) << "removing node " << node->getId() << "from source nodes";
		source_nodes.erase(node);
	}
	if (uncon_nodes.count(node)) {
		uncon_nodes.erase(node);
		Logger(Debug) << "removing node " << node->getId() << "from unconnected nodes";
	}
	fwd_connections.erase(node);
	bwd_connections.erase(node);
	Logger(Debug) << "deleting node " << node->getId();
	delete node;
}

void MapBasedModel::addConnection(NodeConnection *con) {
	cd3assert(con, "connection must no be null");
	Node *source = con->source;
	Node *sink = con->sink;

	sink_nodes.erase(source);
	source_nodes.erase(sink);
	uncon_nodes.erase(source);
	uncon_nodes.erase(sink);

	bwd_connections[sink].push_back(con);
	fwd_connections[source].push_back(con);
	all_connections.insert(con);
}

void MapBasedModel::removeConnection(NodeConnection *con) {
	cd3assert(con, "connection must no be null");
	cd3assert(all_connections.count(con), "no such connection");

	Node *source = con->source;
	Node *sink = con->sink;

	all_connections.erase(con);

	vector<NodeConnection*>::iterator it = find(bwd_connections[sink].begin(),
												bwd_connections[sink].end(),
												con);
	if (it != bwd_connections[sink].end()) {
		bwd_connections[sink].erase(it);
	}

	it = find(fwd_connections[source].begin(),
			  fwd_connections[source].end(),
			  con);
	if (it != fwd_connections[source].end()) {
		fwd_connections[source].erase(it);
	}


	bool no_fwd = false;
	if (fwd_connections[source].size() == 0) {
		sink_nodes.insert(source);
		no_fwd = true;
	}

	if (bwd_connections[source].size() == 0) {
		source_nodes.insert(source);
		if (no_fwd)
			uncon_nodes.insert(source);
	}

	no_fwd = false;
	if (fwd_connections[sink].size() == 0) {
		sink_nodes.insert(sink);
		no_fwd = true;
	}

	if (bwd_connections[sink].size() == 0) {
		source_nodes.insert(sink);
		if (no_fwd)
			uncon_nodes.insert(sink);
	}
}

node_set_type MapBasedModel::initNodes(const SimulationParameters &sp) {
	node_set_type::iterator it = all_nodes.begin();
	node_set_type problem_nodes;
	while (it != all_nodes.end()) {
		Node *n = *it;
		if (!n->init(sp.start, sp.stop, sp.dt)) {
			problem_nodes.insert(n);
		}
		it++;
	}
	return problem_nodes;
}

void MapBasedModel::deinitNodes() {
	node_set_type::iterator it = all_nodes.begin();
	while (it != all_nodes.end()) {
		Node *n = *it;
		n->deinit();
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
		Logger(Warning) << "node" << n->getId() << "not connected";
	}
	return uncon_nodes.empty();
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
				Logger(Warning) << "inport" << port.first << "of node" << node->getId() << "is not connected";
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
				Logger(Warning) << "outport" << port.first << "of node" << node->getId() << "is not connected";
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
		node_set_type reachable;
		if (cycleNodesHelper(n, reachable)) {
			cycle_nodes.insert(n);
		}
	}

	return cycle_nodes;
}

bool MapBasedModel::cycleNodesHelper(Node *n,
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

bool MapBasedModel::renameNode(Node *node, const string &new_id) {
	//TODO assert here
	if (names_nodes.count(new_id)) {
		Logger(Warning) << "new node name already used";
		return false;
	}
	names_nodes.erase(node->getId());
	node->setId(new_id);
	names_nodes[node->getId()] = node;
	return true;
}
