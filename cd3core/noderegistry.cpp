#include "noderegistry.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;

#include "nodefactory.h"
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
	cd3assert(!contains(factory->getNodeName()), "NodeFactory already registered for that name");
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
	cd3assert(contains(name),
			  str(format("no such node class registered: %1%") % name));
	return registered_nodes.find(name)->second->createNode();
}

Node *NodeRegistry::createNode(const std::string &name, const std::string &script) const {
	cd3assert(contains(name), "no such node registered");
	return registered_nodes.find(name)->second->createNode(script);
}

bool NodeRegistry::contains(const std::string &name) const {
	return registered_nodes.find(name) != registered_nodes.end();
}
