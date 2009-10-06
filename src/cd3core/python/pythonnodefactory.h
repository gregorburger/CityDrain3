#ifndef PYTHONNODEFACTORY_H
#define PYTHONNODEFACTORY_H

#include <Python.h>
#include <nodefactory.h>
#include <boost/python.hpp>


struct PNodeFactoryPriv;

class PythonNodeFactory : public INodeFactory
{
public:
	PythonNodeFactory(boost::python::object klass, string name);
	virtual ~PythonNodeFactory();

	Node *createNode(const std::string &s = "") const;
	std::string getNodeName();
private:
	PNodeFactoryPriv *priv;
};

#endif // PYTHONNODEFACTORY_H
