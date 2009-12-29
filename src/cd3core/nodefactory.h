#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <string>
#include <typeinfo>

#include "node.h"
#include "cd3utils.h"
#include <boost/utility.hpp>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>

class CD3_PUBLIC INodeFactory {
public:
	virtual ~INodeFactory(){}
	virtual Node *createNode() const = 0;
	virtual std::string getNodeName() = 0;
};

template <typename T>
class CD3_PUBLIC NodeFactory
	: public INodeFactory {
public:
	NodeFactory();
	virtual Node *createNode() const;
	virtual std::string getNodeName();
};

template <typename T>
NodeFactory<T>::NodeFactory() {
}

template <typename T>
Node *NodeFactory<T>::createNode() const {
	return new T();
}

template <typename T>
std::string NodeFactory<T>::getNodeName() {
	return T::name;
}

#endif // NODEFACTORY_H
