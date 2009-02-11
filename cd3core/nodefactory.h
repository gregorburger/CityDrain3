#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <string>
#include <typeinfo>
#include <boost/shared_ptr.hpp>

#include "node.h"

class INodeFactory {
public:
	virtual boost::shared_ptr<Node> createNode() const = 0;
	virtual std::string getNodeName() = 0;
};

template <typename T>
class NodeFactory
	: public INodeFactory {
public:
	NodeFactory();
	virtual boost::shared_ptr<Node> createNode() const;
	virtual std::string getNodeName();
private:
	std::string nodeName;
};

template <typename T>
NodeFactory<T>::NodeFactory() {
	boost::shared_ptr<Node> tmp = createNode();
	nodeName = tmp->getNodeName();
}

template <typename T>
boost::shared_ptr<Node> NodeFactory<T>::createNode() const {
	return boost::shared_ptr<Node>(new T());
}

template <typename T>
std::string NodeFactory<T>::getNodeName() {
	return nodeName;
}

#endif // NODEFACTORY_H
