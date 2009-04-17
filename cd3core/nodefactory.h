#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <string>
#include <typeinfo>

#include "node.h"
#include "cd3utils.h"
#include <boost/utility.hpp>
#include <cd3globals.h>

class CD3_PUBLIC INodeFactory {
public:
	virtual ~INodeFactory(){}
	virtual Node *createNode(const std::string &s = "") const = 0;
	virtual std::string getNodeName() = 0;
};

template <typename T, bool scripted = false>
class CD3_PUBLIC NodeFactory
	: public INodeFactory {
public:
	NodeFactory();
	virtual Node *createNode(const std::string & = "") const;
	virtual std::string getNodeName();

private:
	Node *createNode(Int2Type<true>, const std::string &s = "") const;
	Node *createNode(Int2Type<false>, const std::string &s = "") const;
};

template <typename T, bool scripted>
NodeFactory<T, scripted>::NodeFactory() {
}

template <typename T, bool scripted>
Node *NodeFactory<T, scripted>::createNode(const std::string &s) const {
	return createNode(Int2Type<scripted>(), s);
}

template <typename T, bool scripted>
std::string NodeFactory<T, scripted>::getNodeName() {
	return T::name;
}

template <typename T, bool scripted>
Node *NodeFactory<T, scripted>::createNode(Int2Type<true>, const std::string &s) const {
	return new T(s);
}

template <typename T, bool scripted>
Node *NodeFactory<T, scripted>::createNode(Int2Type<false>, const std::string &s) const {
	(void) s;
	return new T();
}


#endif // NODEFACTORY_H
