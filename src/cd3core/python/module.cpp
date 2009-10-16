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

	NodeWrapper(PyObject *obj) : self(obj) {
		Py_INCREF(self);
		cout << "creating nodewrapper" << endl;
	}

	virtual ~NodeWrapper() {
		cout << "deleting nodewrapper" << endl;
		//Py_DECREF(self);
	}

	void setSelf(PyObject *_self) {
		cout << "setting self" << endl;
		self = _self;
	}

	int f(int time, int dt) {
		try {
			return call_method<int>(self, "f", time, dt);
			//return this->get_override("f")(time, dt);
		} catch(error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
		}
	}

	void init(int start, int stop, int dt) {
		try {
			call_method<void>(self, "init", start, stop, dt);

			//this->get_override("init")(start, stop, dt);
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
private:
	PyObject *self;
};

BOOST_PYTHON_MODULE(cd3) {
	class_<Node, auto_ptr<NodeWrapper>, boost::noncopyable>("Node")
			.def("f", pure_virtual(&Node::f))
			.def("init", pure_virtual(&Node::init))
			.def("addInPort", &NodeWrapper::addInPort)
			.def("addOutPort", &NodeWrapper::addOutPort)
			.def("bakaddParameter", &Node::addParameter<int>)
			.def("addParameter", &NodeWrapper::pyAddParameter)
			;
	implicitly_convertible<auto_ptr<NodeWrapper>, auto_ptr<Node> >();
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

Node *PythonEnv::createNode(string _name) {
	const char *name = _name.c_str();
}

void PythonEnv::registerNodes(NodeRegistry *registry, const string &module) {
	try {
		object result = exec(
				"import cd3\n"
				"__import__('cdtest', None, None, [], 1)\n"
				"clss = cd3.Node.__subclasses__()\n"
				, priv->main_namespace, priv->main_namespace);
		object clss = priv->main_namespace["clss"];
		for (int i = 0; i < len(clss); i++) {
			registry->addNodeFactory(new PythonNodeFactory(clss[i]));
		}
	} catch(error_already_set const &) {
		cerr << __FILE__ << ":" << __LINE__ << endl;
		PyErr_Print();
		abort();
	}
}
