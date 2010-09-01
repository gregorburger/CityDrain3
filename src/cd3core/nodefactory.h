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
	virtual std::string getNodeName() const = 0;
	virtual std::string getSource() const = 0;
};

template <typename T>
class CD3_PUBLIC NodeFactory
	: public INodeFactory {
public:
	NodeFactory(std::string source);
	virtual Node *createNode() const;
	virtual std::string getNodeName() const;
	virtual std::string getSource() const;
private:
	std::string source;
};

template <typename T>
NodeFactory<T>::NodeFactory(std::string source) : source(source) {
}

template <typename T>
Node *NodeFactory<T>::createNode() const {
	return new T();
}

template <typename T>
std::string NodeFactory<T>::getNodeName() const {
	return T::class_name;
}

template <typename T>
std::string NodeFactory<T>::getSource() const {
	return source;
}

#endif // NODEFACTORY_H
