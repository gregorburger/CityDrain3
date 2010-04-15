#include "pythonnodefactory.h"
#include "module.h"
#include "pynode.h"

using namespace boost::python;

struct PNodeFactoryPriv {
	object klass;
	string name;
};

PythonNodeFactory::PythonNodeFactory(object klass) {
	priv = new PNodeFactoryPriv();
	priv->klass = klass;
	priv->name = extract<string>(priv->klass.attr("__name__"));
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
		Node *n = apn.get();
		apn.release();
		return n;
	} catch(error_already_set const &) {
		Logger(Error) << __FILE__ << ":" << __LINE__;
		PyErr_Print();
		abort();
	}
	return 0;
}

std::string PythonNodeFactory::getNodeName() {
	return priv->name;
}

