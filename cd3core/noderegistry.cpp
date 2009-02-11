#include "noderegistry.h"

#include "nodefactory.h"
#include "node.h"

#include <memory>
#include <boost/foreach.hpp>

NodeRegistry::NodeRegistry()
{
}

NodeRegistry::~NodeRegistry() {
	std::pair<std::string, boost::shared_ptr<INodeFactory> > p;
	BOOST_FOREACH(p, registered_nodes) {
		p.second.reset();
	}
}

bool NodeRegistry::addNodeFactory(boost::shared_ptr<INodeFactory> factory) {
	if (registered_nodes.find(factory.get()->getNodeName()) != registered_nodes.end()) {
		return false;
	}
	registered_nodes[factory->getNodeName()] = factory;
	return true;
}

std::list<std::string> NodeRegistry::getRegisteredNames() const {
	std::list<std::string> names;

	for (reg_node_type::const_iterator it = registered_nodes.begin();
		it != registered_nodes.end(); it++) {
		names.push_back(it->first);
	}

	return names;
}

boost::shared_ptr<Node> NodeRegistry::createNode(const std::string &name) {
	return registered_nodes[name]->createNode();
}

bool NodeRegistry::contains(const std::string &name) const {
	return registered_nodes.find(name) != registered_nodes.end();
}
