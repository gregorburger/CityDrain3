#include "pythonnodefactory.h"
#include "module.h"

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
	Node *n;
	try {
		object node = priv->klass();
		set_self(node);
		PyObject *pyobj = node.ptr();
		Py_INCREF(pyobj);
		n = extract<Node*>(node);
	} catch(error_already_set const &) {
		PyErr_Print();
	}
	return n;
}

std::string PythonNodeFactory::getNodeName() {
	return priv->name;
}

