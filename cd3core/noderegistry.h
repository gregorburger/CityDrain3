#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <string>
#include <cd3globals.h>

class INodeFactory;
class Node;

typedef std::map<std::string, INodeFactory *> reg_node_type;

class CD3_PUBLIC NodeRegistry
{
public:
	NodeRegistry();
	~NodeRegistry();

	bool addNodeFactory(INodeFactory *factory);

	//std::list<std::string> getRegisteredNames() const;
	Node *createNode(const std::string &name) const;
	Node *createNode(const std::string &name, const std::string &script) const;
	bool contains(const std::string &name) const;

private:
	reg_node_type registered_nodes;
};

#endif // NODEREGISTRY_H
