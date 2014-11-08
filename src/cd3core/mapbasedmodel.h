/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#ifndef MAPBASEMODEL_H
#define MAPBASEMODEL_H

#include <imodel.h>

#include <set>
#include <cd3globals.h>

typedef std::map<Node *, std::vector<NodeConnection *> > connection_type;

class CD3_PUBLIC MapBasedModel : public IModel
{
public:
	MapBasedModel();
	~MapBasedModel();

	std::string serialize() { //TODO implement
		return "";
	}

	void deserialize(const std::string &serialid) { //TODO implement
		(void) serialid;
	}

	bool empty() const { return all_nodes.size() == 0; }

	void addNode(const std::string &id, Node *node);
	void removeNode(Node *node);
	bool renameNode(Node *node, const std::string &new_id);
	void addConnection(NodeConnection *con);
	void removeConnection(NodeConnection *con);


	//call before adding Connections
	node_set_type initNodes(const SimulationParameters &);
	void deinitNodes();

	node_set_type getSourceNodes();
	const node_set_type getSourceNodes() const;
	node_set_type getSinkNodes();

	std::vector<NodeConnection *> forwardConnection(Node *n);
	const std::vector<NodeConnection *> forwardConnection(Node *n) const;
	std::vector<NodeConnection *> backwardConnection(Node *n);

	const node_set_type *getNodes() const;
	const con_set_type *getConnections() const;

	name_node_map getNamesAndNodes() const;

	Node *getNode(const std::string &name) const;
	con_count_type getForwardCounts() const;
	con_count_type getBackwardCounts() const;

	bool connected() const;
	void checkModel() const;
	bool cycleFree() const;
	node_set_type cycleNodes() const;

private:
	bool cycleNodesHelper(Node *n, node_set_type) const;

	node_set_type all_nodes;
	node_set_type sink_nodes;
	node_set_type source_nodes;
	node_set_type uncon_nodes;
	name_node_map names_nodes;
	connection_type fwd_connections;
	connection_type bwd_connections;
	std::set<NodeConnection*> all_connections;
};

#endif // MAPBASEMODEL_H
