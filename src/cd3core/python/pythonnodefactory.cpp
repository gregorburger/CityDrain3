#include "pythonnodefactory.h"
#include "module.h"

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

Node *PythonNodeFactory::createNode(const std::string &) const {
	try {
		object node = priv->klass();
		auto_ptr<Node> nn = extract<auto_ptr<Node> >(node);
		Node *n = nn.get();
		nn.release();
		return n;
	} catch(error_already_set const &) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		PyErr_Print();
		abort();
	}
	return 0;
}

std::string PythonNodeFactory::getNodeName() {
	return priv->name;
}

