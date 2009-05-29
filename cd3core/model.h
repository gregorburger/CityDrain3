#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <boost/unordered/unordered_map.hpp>
#include <boost/unordered/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include <cd3globals.h>

class Node;
struct SimulationParameters;
struct NodeConnection;

using namespace boost;
using namespace std;

typedef unordered_map<string, Node *> name_node_map;
typedef unordered_set<Node *> node_set_type;
typedef pair<Node *, string> end_point_type;
typedef tuple<string, Node *, string> next_node_type;
typedef unordered_map<Node *, int> con_count_type;

class CD3_PUBLIC IModel {
public:
	virtual ~IModel(){}

	virtual string serialize() = 0;
	virtual void deserialize(const string &serialid) = 0;

	virtual void addNode(const string &name, Node *node) = 0;
	virtual void addConnection(const string &src_node,
					   const string &src_port,
					   const string &sin_node,
					   const string &sin_port) = 0;

	virtual void initNodes(const SimulationParameters &) = 0;
	virtual node_set_type getSourceNodes() = 0;
	virtual node_set_type getSinkNodes() = 0;

	virtual vector<NodeConnection *> forwardConnection(Node *n) = 0;
	virtual vector<NodeConnection *> backwardConnection(Node *n) = 0;
	virtual con_count_type getForwardCounts() const = 0;
	virtual con_count_type getBackwardCounts() const = 0;

	virtual const node_set_type *getNodes() const = 0;
	virtual name_node_map getNamesAndNodes() const = 0;
	virtual Node *getNode(const string &name) const = 0;
	virtual string getNodeName(const Node *node) const = 0;

	virtual bool connected() const = 0;
	virtual void checkModel() const = 0;
	virtual bool cycleFree() const = 0;
};

#endif // MODEL_H
