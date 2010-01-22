#ifndef MODEL_H
#define MODEL_H

#define USE_HASH

#include <string>
#include <vector>
#ifdef USE_HASH
#include <boost/unordered/unordered_map.hpp>
#include <boost/unordered/unordered_set.hpp>
#else
#include <map>
#include <set>
using namespace std;
#define unordered_map map
#define unordered_set set
#endif
#include <boost/tuple/tuple.hpp>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>


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
typedef unordered_set<NodeConnection *> con_set_type;

class CD3_PUBLIC IModel {
public:
	virtual ~IModel(){}

	virtual string serialize() = 0;
	virtual void deserialize(const string &serialid) = 0;

	virtual void addNode(Node *node) = 0;
	virtual void removeNode(Node *node) = 0;
	virtual void addConnection(NodeConnection *con) = 0;
	virtual void removeConnection(NodeConnection *con) = 0;

	virtual void initNodes(const SimulationParameters &) = 0;
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
