#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <string>

class INodeFactory;//#include "nodefactory.h"
class Node;

typedef std::map<std::string, INodeFactory *> reg_node_type;

class NodeRegistry
{
public:
	NodeRegistry();
	~NodeRegistry();

	bool addNodeFactory(INodeFactory *factory);

	//std::list<std::string> getRegisteredNames() const;
	Node *createNode(const std::string &name);
	Node *createNode(const std::string &name, const std::string &script);
	bool contains(const std::string &name) const;

private:
	reg_node_type registered_nodes;
};

#endif // NODEREGISTRY_H
