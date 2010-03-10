#ifndef MAPBASEMODEL_H
#define MAPBASEMODEL_H

#include <imodel.h>

#include <set>
#include <cd3globals.h>

typedef unordered_map<Node *, vector<NodeConnection *> > connection_type;

class CD3_PUBLIC MapBasedModel : public IModel
{
public:
	MapBasedModel();
	~MapBasedModel();

	string serialize() { //TODO implement
		return "";
	}

	void deserialize(const string &serialid) { //TODO implement
		(void) serialid;
	}

	bool empty() const { return all_nodes.size() == 0; }

	void addNode(Node *node);
	void removeNode(Node *node);
	void addConnection(NodeConnection *con);
	void removeConnection(NodeConnection *con);


	//call before adding Connections
	bool initNodes(const SimulationParameters &);
	void deinitNodes();

	node_set_type getSourceNodes();
	const node_set_type getSourceNodes() const;
	node_set_type getSinkNodes();

	vector<NodeConnection *> forwardConnection(Node *n);
	const vector<NodeConnection *> forwardConnection(Node *n) const;
	vector<NodeConnection *> backwardConnection(Node *n);

	const node_set_type *getNodes() const;
	const con_set_type *getConnections() const;

	name_node_map getNamesAndNodes() const;

	Node *getNode(const string &name) const;
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
	unordered_set<NodeConnection*> all_connections;
};

#endif // MAPBASEMODEL_H
