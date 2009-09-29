#include "pythonnodefactory.h"
#include <boost/python.hpp>

using namespace boost::python;

struct PNodeFactoryPriv {
	object klass;
	string name;
};

PythonNodeFactory::PythonNodeFactory(object klass, string name) {
	priv = new PNodeFactoryPriv();
	priv->klass = klass;
	priv->name = name;
}

PythonNodeFactory::~PythonNodeFactory() {
	delete priv;
}

Node *PythonNodeFactory::createNode(const std::string &) const {
	try {
		object node = priv->klass();
		return extract<Node*>(node);
	} catch(error_already_set const &) {
		PyErr_Print();
	}
}

std::string PythonNodeFactory::getNodeName() {
	cout << "getnodename" << endl;
	return priv->name;
}

