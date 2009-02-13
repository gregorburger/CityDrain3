#ifndef MAPBASEMODEL_H
#define MAPBASEMODEL_H

#include "model.h"

#include <map>
#include <set>

typedef std::map<std::string, Node *> node_map;
typedef std::pair<Node *, std::string> end_point_type;
typedef std::map<end_point_type, end_point_type> connection_type;
typedef std::set<Node *> node_set_type;

class MapBasedModel : public IModel
{
public:
	MapBasedModel();
	~MapBasedModel();
	void addNode(const std::string &name, Node *node);
	void addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port);
	void dump();


	//call before adding Connections
	void initNodes(const SimulationParameters &);

	node_set_type sourceNodes();
	node_set_type sinkNodes();

private:
	node_set_type all_nodes;
	node_set_type sink_nodes;
	node_set_type source_nodes;
	node_map names_nodes;
	connection_type fwd_connections;

};

#endif // MAPBASEMODEL_H
