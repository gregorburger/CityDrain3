#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <string>
#include <typeinfo>

#include "node.h"
#include <boost/utility.hpp>

class INodeFactory {
private:
	virtual Node *createNode() const = 0;
	virtual std::string getNodeName() = 0;
public:
	virtual ~INodeFactory(){}
	Node *doCreateNode() {
		return createNode();
	}

	std::string doGetNodeName() {
		return getNodeName();
	}
};

template <typename T>
class NodeFactory
	: public INodeFactory {
public:
	NodeFactory();
private:
	virtual Node *createNode() const;
	virtual std::string getNodeName();
	std::string nodeName;
};

template <typename T>
NodeFactory<T>::NodeFactory() {
	Node *tmp = createNode();
	nodeName = tmp->getNodeName();
	delete tmp;
}

template <typename T>
Node *NodeFactory<T>::createNode() const {
	return (new T());
}

template <typename T>
std::string NodeFactory<T>::getNodeName() {
	return nodeName;
}

#endif // NODEFACTORY_H
