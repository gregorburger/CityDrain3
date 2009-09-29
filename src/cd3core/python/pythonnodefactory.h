#ifndef PYTHONNODEFACTORY_H
#define PYTHONNODEFACTORY_H

#include <nodefactory.h>
#include <boost/python.hpp>

using namespace boost::python;

struct PNodeFactoryPriv;

class PythonNodeFactory : public INodeFactory
{
public:
	PythonNodeFactory(object klass, string name);
	virtual ~PythonNodeFactory();

	Node *createNode(const std::string &s = "") const;
	std::string getNodeName();
private:
	PNodeFactoryPriv *priv;
};

#endif // PYTHONNODEFACTORY_H
