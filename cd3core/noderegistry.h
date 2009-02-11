#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <string>

class INodeFactory;//#include "nodefactory.h"
class Node;

typedef std::map<std::string, INodeFactory *> reg_node_type;

class INodeRegistry {
private:
	virtual bool addNodeFactory(INodeFactory *factory) = 0;
	//virtual std::list<std::string> getRegisteredNames() const = 0;
	virtual Node *createNode(const std::string &name) = 0;
	virtual bool contains(const std::string &name) const = 0;

public:
	virtual ~INodeRegistry() {
	}
	Node *doCreateNode(const std::string &name) {
		return createNode(name);
	}

	bool doAddNodeFactory(INodeFactory *factory) {
		return addNodeFactory(factory);
	}

	bool doContains(const std::string &name) {
		return contains(name);
	}
};

class NodeRegistry : public INodeRegistry
{
public:
	NodeRegistry();
	~NodeRegistry();

private:
	bool addNodeFactory(INodeFactory *factory);

	//std::list<std::string> getRegisteredNames() const;
	Node *createNode(const std::string &name);
	bool contains(const std::string &name) const;


	reg_node_type registered_nodes;
};

#endif // NODEREGISTRY_H
