#ifndef PYTHONNODEFACTORY_H
#define PYTHONNODEFACTORY_H

#include <nodefactory.h>
#include <boost/python.hpp>

struct PNodeFactoryPriv;

class PythonNodeFactory : public INodeFactory
{
public:
	PythonNodeFactory(std::string module, boost::python::object klass);
	virtual ~PythonNodeFactory();

	Node *createNode() const;
	std::string getNodeName() const;
	std::string getSource() const;
private:
	PNodeFactoryPriv *priv;
};

#endif // PYTHONNODEFACTORY_H
