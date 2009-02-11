#include "noderegistry.h"

#include "nodefactory.h"
#include "node.h"
#include <boost/foreach.hpp>
#include <iostream>

NodeRegistry::NodeRegistry() {
}

typedef std::pair<std::string, INodeFactory *> rn_ptype;

NodeRegistry::~NodeRegistry() {
	BOOST_FOREACH(rn_ptype t, registered_nodes) {
		delete t.second;
	}
}

bool NodeRegistry::addNodeFactory(INodeFactory *factory) {
	assert(registered_nodes.find(factory->doGetNodeName()) == registered_nodes.end());
	registered_nodes[factory->doGetNodeName()] = factory;
	return true;
}

/*std::list<std::string> NodeRegistry::getRegisteredNames() const {
	std::list<std::string> names;

	for (reg_node_type::const_iterator it = registered_nodes.begin();
		it != registered_nodes.end(); it++) {
		names.push_back(it->first);
	}

	return names;
}*/

Node *NodeRegistry::createNode(const std::string &name) {
	return registered_nodes[name]->doCreateNode();
}

bool NodeRegistry::contains(const std::string &name) const {
	return registered_nodes.find(name) != registered_nodes.end();
}
