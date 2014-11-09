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

#ifndef MODEL_H
#define MODEL_H

//#define USE_HASH

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/tuple/tuple.hpp>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>


class Node;
struct SimulationParameters;
struct NodeConnection;

using namespace boost;
using namespace std;

typedef std::map<string, Node *> name_node_map;
typedef std::set<Node *> node_set_type;
typedef pair<Node *, string> end_point_type;
typedef boost::tuple<string, Node *, string> next_node_type;
typedef std::map<Node *, int> con_count_type;
typedef std::set<NodeConnection *> con_set_type;

class CD3_PUBLIC IModel {
public:
	virtual ~IModel(){}

	virtual string serialize() = 0;
	virtual void deserialize(const string &serialid) = 0;

	virtual void addNode(const string &id, Node *node) = 0;
	virtual bool renameNode(Node *node, const string &new_id) = 0;
	virtual void removeNode(Node *node) = 0;
	virtual void addConnection(NodeConnection *con) = 0;
	virtual void removeConnection(NodeConnection *con) = 0;

	virtual node_set_type initNodes(const SimulationParameters &) = 0;
	virtual void deinitNodes() = 0;

	virtual node_set_type getSourceNodes() = 0;
	virtual node_set_type getSinkNodes() = 0;

	virtual vector<NodeConnection *> forwardConnection(Node *n) = 0;
	virtual vector<NodeConnection *> backwardConnection(Node *n) = 0;
	virtual con_count_type getForwardCounts() const = 0;
	virtual con_count_type getBackwardCounts() const = 0;

	virtual const node_set_type *getNodes() const = 0;
	virtual const con_set_type *getConnections() const = 0;
	virtual name_node_map getNamesAndNodes() const = 0;
	virtual Node *getNode(const string &name) const = 0;

	virtual bool connected() const = 0;
	virtual void checkModel() const = 0;
	virtual bool cycleFree() const = 0;
	virtual node_set_type cycleNodes() const = 0;
};

#endif // MODEL_H
