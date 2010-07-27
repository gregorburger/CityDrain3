#include <Python.h>
#include "pythonnodefactory.h"
#include "module.h"
#include "pynode.h"
#include "pythonexception.h"

using namespace boost::python;

struct PNodeFactoryPriv {
	object klass;
	std::string name;
	std::string module;
};

PythonNodeFactory::PythonNodeFactory(string module, object klass) {
	priv = new PNodeFactoryPriv();
	priv->klass = klass;
	priv->name = extract<std::string>(priv->klass.attr("__name__"));
	priv->module = module;
}

PythonNodeFactory::~PythonNodeFactory() {
	delete priv;
}

Node *PythonNodeFactory::createNode() const {
	try {
		object node = priv->klass();
		auto_ptr<NodeWrapper> apn = extract<auto_ptr<NodeWrapper> >(node);
		apn->setClassName(priv->name);
		apn->setSelf(node);
		return apn.release();
	} catch(error_already_set const &) {
		Logger(Error) << __FILE__ << ":" << __LINE__;
		handle_python_exception();
	}
	return 0;
}

std::string PythonNodeFactory::getNodeName() const {
	return priv->name;
}

std::string PythonNodeFactory::getSource() const {
	return priv->module + " (Python)";
}

