#include <Python.h>
#include "module.h"
#include "../node.h"
#include <flow.h>
#include <noderegistry.h>
#include "pythonnodefactory.h"
#include <calculationunit.h>

#include <boost/foreach.hpp>
#include <boost/python.hpp>

using namespace boost::python;

void test_flow(Flow *f) {
	assert(f);
	cout << "test_flow" << endl;
	BOOST_FOREACH(string n, f->getNames()) {
		cout << "test_flow" << n << endl;
	}
	f->addUnit("test", CU::flow, 43.0);
}

void test_node(Node *n) {
	n->init(0, 900, 300);
	n->f(0, 300);
	n->f(300, 300);
}

struct NodeWrapper : Node, wrapper<Node> {

	virtual ~NodeWrapper() {
		cout << "deleting nodewrapper" << endl;
		Py_DECREF(self);
	}

	void setSelf(PyObject *_self) {
		cout << "setting self" << endl;
		self = _self;
	}

	int f(int time, int dt) {
		try {
			return this->get_override("f")(time, dt);
		} catch(error_already_set const &) {
			PyErr_Print();
		}
	}

	void init(int start, int stop, int dt) {
		try {
			this->get_override("init")(start, stop, dt);
		} catch(error_already_set const &) {
			PyErr_Print();
		}
	}

	const char *getClassName() const {
		return "";
	}

	void addInPort(const std::string &name, Flow *inflow) {
		Node::addInPort(name, inflow);
	}

	void addOutPort(const std::string &name, Flow *outflow) {
		Node::addOutPort(name, outflow);
	}
private:
	PyObject *self;
};

void set_self(object o) {
	NodeWrapper *nw = extract<NodeWrapper *>(o);
	PyObject *obj = o.ptr();
	nw->setSelf(obj);
}

BOOST_PYTHON_MODULE(cd3) {
	class_<NodeWrapper, boost::noncopyable>("Node")
			.def("f", pure_virtual(&Node::f))
			.def("init", pure_virtual(&Node::init))
			.def("addInPort", &NodeWrapper::addInPort)
			.def("addOutPort", &NodeWrapper::addOutPort)
			;
	class_<Flow>("Flow")
			;
	def("test_node", &test_node);
	def("test_flow", &test_flow);
}

struct PythonEnvPriv {
	object main_module, main_namespace;
};

PythonEnv *PythonEnv::instance = 0;

PythonEnv::PythonEnv() {
	priv = new PythonEnvPriv();
	Py_Initialize();
	initcd3();
	priv->main_module = import("__main__");
	priv->main_namespace = priv->main_module.attr("__dict__");
}

PythonEnv::~PythonEnv() {
	delete priv;
}

PythonEnv *PythonEnv::getInstance() {
	if (instance == 0) {
		instance = new PythonEnv();
	}

	return instance;
}

Node *PythonEnv::createNode(string _name) {
	const char *name = _name.c_str();
}

void PythonEnv::registerNodes(NodeRegistry *registry, const string &module) {
	/*const char *prog = "import cd3\n"
						"__import__('cdtest', None, None, [], 1)\n"
						"clss = cd3.Node.__subclasses__()\n";

	object result = exec(prog,
						 priv->main_namespace,
						 priv->main_namespace);

	object clss = priv->main_namespace["clss"];
	try {
		cout << "classes " << len(clss) << endl;

		for (int i = 0; i < len(clss); i++) {
			object pKlass = clss[i];
			object pName = pKlass["__name__"];
			string cname = extract<string>(pName);
			registry->addNodeFactory(new PythonNodeFactory(pKlass, cname));
		}
		} catch(error_already_set const &) {
			PyErr_Print();
		}
	}*/

	try {
		object result = exec(
				"import cd3\n"
				"__import__('cdtest', None, None, [], 1)\n"
				"clss = cd3.Node.__subclasses__()\n"
				, priv->main_namespace, priv->main_namespace);
		object clss = priv->main_namespace["clss"];
		for (int i = 0; i < len(clss); i++) {
			string cname = extract<string>(clss[i].attr("__name__"));
			registry->addNodeFactory(new PythonNodeFactory(clss[i], cname));
		}
	} catch(error_already_set const &) {
		PyErr_Print();
	}

}
