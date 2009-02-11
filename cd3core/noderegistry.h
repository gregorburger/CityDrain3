#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

#include "nodefactory.h"
class Node;

typedef std::map<std::string, boost::shared_ptr<INodeFactory> > reg_node_type;

class INodeRegistry {
public:
	virtual bool addNodeFactory(boost::shared_ptr<INodeFactory> factory) = 0;
	virtual std::list<std::string> getRegisteredNames() const = 0;
	virtual boost::shared_ptr<Node> createNode(const std::string &name) = 0;
	virtual bool contains(const std::string &name) const = 0;
};

class NodeRegistry : public INodeRegistry
{
public:
    NodeRegistry();
	~NodeRegistry();
	bool addNodeFactory(boost::shared_ptr<INodeFactory> factory);

	std::list<std::string> getRegisteredNames() const;
	boost::shared_ptr<Node> createNode(const std::string &name);
	bool contains(const std::string &name) const;

private:
	reg_node_type registered_nodes;
};

#endif // NODEREGISTRY_H
