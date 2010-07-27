#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <string>
#include <vector>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

class INodeFactory;
class Node;

typedef std::map<std::string, INodeFactory *> reg_node_type;

class CD3_PUBLIC NodeRegistry
{
public:
	NodeRegistry();
	~NodeRegistry();

	bool addNodeFactory(INodeFactory *factory);
	void addNativePlugin(const std::string &plugin_path);

	std::vector<std::string> getRegisteredNames() const;

	std::map<std::string, std::vector<std::string> >
	getRegisteredNamesAndSources() const;

	Node *createNode(const std::string &name) const;
	bool contains(const std::string &name) const;

private:
	reg_node_type registered_nodes;
};

typedef void (*regNodeFunProto) (NodeRegistry *reg);

#endif // NODEREGISTRY_H
