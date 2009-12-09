#include "module.h"
#include "../node.h"
#include <flow.h>
#include <noderegistry.h>
#include "pythonnodefactory.h"

#include <boost/foreach.hpp>
#include <boost/python.hpp>

using namespace boost::python;

void test_flow(Flow *f) {
	assert(f);
	cout << "test_flow" << endl;
	BOOST_FOREACH(string n, f->getNames()) {
		cout << "test_flow" << n << endl;
	}
}


struct NodeWrapper : Node, wrapper<Node> {
	int f(int time, int dt) {
		try {
			return this->get_override("f")(time, dt);
		} catch(error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
		}
	}

	void pushInStates() {

	}

	void pullOutStates() {

	}

	void init(int start, int stop, int dt) {
		try {		
			this->get_override("init")(start, stop, dt);
		} catch(error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
		}
	}

	const char *getClassName() const {
		object o(this);
		const char *name = extract<const char*>(o.attr("__name__"));
		return name;
	}

	void addInPort(const std::string &name, Flow *inflow) {
		Node::addInPort(name, inflow);
	}

	void addOutPort(const std::string &name, Flow *outflow) {
		Node::addOutPort(name, outflow);
	}

	void pyAddParameter(object value) {
		string name = extract<string>(value.attr("__name__"));
		cout << "parameter " << name << endl;
	}
};

void test_node(shared_ptr<NodeWrapper> n) {
	n->init(0, 900, 300);
	n->f(0, 300);
	n->f(300, 300);
}

BOOST_PYTHON_MODULE(cd3) {
	class_<NodeWrapper, shared_ptr<NodeWrapper>, boost::noncopyable>("Node")
			.def("f", pure_virtual(&Node::f))
			.def("init", pure_virtual(&Node::init))
			.def("addInPort", &NodeWrapper::addInPort)
			.def("addOutPort", &NodeWrapper::addOutPort)
			;
	class_<Flow>("Flow")
			;
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

void PythonEnv::freeInstance() {
	if (instance == 0)
		return;
	exec("import gc\n"
		 "print gc.collect()\n"
		 "print 'where is the garbage'\n"
		 "print gc.garbage\n",
		 instance->priv->main_namespace,instance->priv->main_namespace);

	delete instance;
	Py_Finalize();
}

void PythonEnv::registerNodes(NodeRegistry *registry, const string &module) {
	try {
		object result = exec(
				"import cd3\n"
				"__import__('cdtest', None, None, [], 1)\n"
				"clss = cd3.Node.__subclasses__()\n"
				, priv->main_namespace, priv->main_namespace);
		object clss = priv->main_namespace["clss"];
		cout << "found " << len(clss) << " Nodes in module " << module << endl;
		for (int i = 0; i < len(clss); i++) {
			registry->addNodeFactory(new PythonNodeFactory(clss[i]));
		}
	} catch(error_already_set const &) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		PyErr_Print();
		abort();
	}
}
