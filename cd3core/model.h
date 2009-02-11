#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

class Node;

typedef std::map<std::string, Node *> node_map;

class Model
{
public:
    Model();
	~Model();
	void addNode(const std::string &name, Node *node);
	void addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port);
	void dump();

private:
	node_map nodes;

};

#endif // MODEL_H
