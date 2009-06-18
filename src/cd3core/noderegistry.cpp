#include "noderegistry.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <QLibrary>
#include <QDebug>

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

void NodeRegistry::addPlugin(std::string plugin_path) {
	QLibrary l(QString::fromStdString(plugin_path));
	bool loaded = l.load();
	cd3assert(loaded, str(format("could not load plugin %1%: %2%")
						  % plugin_path
						  % l.errorString().toStdString()));
	regNodeFunProto regNodeFun = (regNodeFunProto) l.resolve("registerNodes");
	if (regNodeFun) {
		regNodeFun(this);
	} else {
		qWarning() << QString::fromStdString(plugin_path) << " has no node register hook";
	}
}

typedef std::pair<std::string, INodeFactory *> snf;
std::vector<std::string> NodeRegistry::getRegisteredNames() const {
	std::vector<std::string> names;

	BOOST_FOREACH(snf item, registered_nodes) {
		names.push_back(item.first);
	}

	return names;
}

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
