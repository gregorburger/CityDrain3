#ifndef PYTHONNODEFACTORY_H
#define PYTHONNODEFACTORY_H

#include <nodefactory.h>
#include <boost/python.hpp>

struct PNodeFactoryPriv;

class PythonNodeFactory : public INodeFactory
{
public:
	PythonNodeFactory(boost::python::object klass);
	virtual ~PythonNodeFactory();

	shared_ptr<Node> createNode(const std::string &s = "") const;
	std::string getNodeName();
private:
	PNodeFactoryPriv *priv;
};

#endif // PYTHONNODEFACTORY_H
