#include "noderegistry.h"

#include "nodefactory.h"
#include <boost/foreach.hpp>

#include <cd3assert.h>

NodeRegistry::NodeRegistry() {
}

typedef std::pair<std::string, INodeFactory *> rn_ptype;

NodeRegistry::~NodeRegistry() {
	BOOST_FOREACH(rn_ptype t, registered_nodes) {
		delete t.second;
	}
}

bool NodeRegistry::addNodeFactory(INodeFactory *factory) {
	assert(!contains(factory->getNodeName()), "NodeFactory already registered for that name");
	registered_nodes[factory->getNodeName()] = factory;
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

Node *NodeRegistry::createNode(const std::string &name) const {
	assert(contains(name), "no such node registered");
	return registered_nodes.find(name)->second->createNode();
}

Node *NodeRegistry::createNode(const std::string &name, const std::string &script) const {
	assert(contains(name), "no such node registered");
	return registered_nodes.find(name)->second->createNode(script);
}

bool NodeRegistry::contains(const std::string &name) const {
	return registered_nodes.find(name) != registered_nodes.end();
}
