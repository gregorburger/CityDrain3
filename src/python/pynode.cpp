#include "pynode.h"
#include "../cd3core/node.h"
#include <noderegistry.h>
#include <nodefactory.h>
#include <flow.h>
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <nodefactory.h>

using namespace boost;

struct NodeWrapper : Node, python::wrapper<Node> {
	NodeWrapper(PyObject *_self) : self(_self) {
		Py_INCREF(self);
		PyObject *klass = PyObject_GetAttrString(self, "__class__");
		PyObject *name = PyObject_GetAttrString(klass, "__name__");
		class_name = PyString_AsString(name);
	}

	virtual ~NodeWrapper() {
		//Py_DECREF(self); FIXME something is double clean up LEAK HERE
	}

	int f(ptime time, int dt) {
		updateParameters();
		try {
			return python::call_method<int>(self, "f", time, dt);
		} catch(python::error_already_set const &) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			PyErr_Print();
			abort();
		}
	}

	void pushInStates() {

	}

	void pullOutStates() {

	}

	bool init(ptime start, ptime stop, int dt) {
		try {
			updateParameters();
			return python::call_method<bool>(self, "init", start, stop, dt);
		} catch(python::error_already_set const &) {
		}
		return true;
	}

	void deinit() {
		try {
			python::call_method<void>(self, "deinit");
		} catch(python::error_already_set const &) {
		}
	}

	const char *getClassName() const {
		return class_name;
	}

	void addInPort(const std::string &name, Flow *inflow) {
		Node::addInPort(name, inflow);
	}

	void addOutPort(const std::string &name, Flow *outflow) {
		Node::addOutPort(name, outflow);
	}

	template<typename T>
	T getRefParameter(string name) {
		return *getParameter<T>(name);
	}

	typedef pair<string, int> intp;
	typedef pair<string, string> stringp;
	typedef pair<string, float> floatp;
	typedef pair<string, double> doublep;
	typedef pair<string, bool> boolp;
	void updateParameters() {
		python::object self_obj = python::object(python::handle<>(python::borrowed(self)));
		python::dict self_dict = python::extract<python::dict>(self_obj.attr("__dict__"));


		BOOST_FOREACH(intp i, int_params) {
			self_dict[i.first] = i.second;
		}

		BOOST_FOREACH(stringp i, string_params) {
			self_dict[i.first] = i.second;
		}

		BOOST_FOREACH(doublep i, double_params) {
			self_dict[i.first] = i.second;
		}

		BOOST_FOREACH(boolp i, bool_params) {
			self_dict[i.first] = i.second;
		}
	}

	void pyAddParameters() {
		python::object s = python::object(python::handle<>(python::borrowed(self)));
		python::dict param = python::extract<python::dict>(s.attr("__dict__"));

		python::list keys = param.keys();
		for (int i = 0; i < len(keys); i++) {
			string key = python::extract<string>(keys[i]);

			if (key == "self") {
				continue;
			}
			python::extract<int> ix(param[key]);
			if (ix.check()) {
				Logger(Debug) << this << "adding int parameter " << key;
				int value = ix;
				int_params[key] = value;
				addParameter(key, &int_params[key]);
				continue;
			}

			python::extract<string> sx(param[key]);
			if (sx.check()) {
				Logger(Debug) << this << "adding string parameter " << key;
				string value = sx;
				string_params[key] = value;
				addParameter(key, &string_params[key]);
				continue;
			}

			python::extract<double> dx(param[key]);
			if (dx.check()) {
				Logger(Debug) << this << "adding double parameter " << key;
				double value = dx;
				double_params[key] = value;
				addParameter(key, &double_params[key]);
				continue;
			}

			python::extract<float> fx(param[key]);
			if (fx.check()) {
				Logger(Debug) << this << "adding float parameter " << key;
				float value = fx;
				float_params[key] = value;
				addParameter(key, &float_params[key]);
				continue;
			}

			python::extract<bool> bx(param[key]);
			if (bx.check()) {
				Logger(Debug) << this << "adding bool parameter " << key;
				bool value = bx;
				bool_params[key] = value;
				addParameter(key, &bool_params[key]);
				continue;
			}
			cout << "unsupported type of param " << key << endl;
		}
	}

	map<string, int> int_params;
	map<string, string> string_params;
	map<string, double> double_params;
	map<string, bool> bool_params;
	map<string, float> float_params;
	PyObject *self;
	char *class_name;
};

typedef pair<string, NodeParameter*> param_pair;
static python::list n_getParameterNames(Node &n) {
	python::list l;
	BOOST_FOREACH(param_pair p, n.getParameters()) {
		l.append(p.first);
	}
	return l;
}

static python::list nr_getRegisteredNames(NodeRegistry &nr) {
	python::list names;
	BOOST_FOREACH(string name, nr.getRegisteredNames()) {
		names.append(name);
	}
	return names;
}

struct INodeFactoryWrapper : public INodeFactory, python::wrapper<INodeFactory> {
	INodeFactoryWrapper(PyObject *_self) : self(_self) {
		Py_INCREF(self);
	}

	virtual ~INodeFactoryWrapper() {
		Py_DECREF(self);
	}

	Node *createNode() const {
		try {
			return python::call_method<Node*>(self, "createNode");
		} catch(python::error_already_set const &) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			PyErr_Print();
			abort();
		}
	}

	string getNodeName() {
		try {
			return python::call_method<string>(self, "getNodeName");
		} catch(python::error_already_set const &) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			PyErr_Print();
			abort();
		}
	}
	PyObject *self;
};

/** transfer ownership*/
static void nr_addNodeFactory(NodeRegistry &nr, auto_ptr<INodeFactory> nf) {
	nr.addNodeFactory(nf.get());
	nf.release();
}

void wrap_node() {
	python::class_<INodeFactory, auto_ptr<INodeFactoryWrapper>, boost::noncopyable>("NodeFactory")
		.def("createNode", pure_virtual(&INodeFactoryWrapper::createNode), python::return_value_policy<python::manage_new_object>())
		.def("getNodeName", pure_virtual(&INodeFactoryWrapper::getNodeName))
		;
	python::implicitly_convertible<auto_ptr<INodeFactoryWrapper>, auto_ptr<INodeFactory> >();
	python::class_<Node, auto_ptr<NodeWrapper>, boost::noncopyable>("Node")
		.def("f", python::pure_virtual(&Node::f))
		.def("init", &Node::init, &NodeWrapper::init)
		.def("deinit", &Node::deinit, &NodeWrapper::deinit)
		.def("addInPort", &NodeWrapper::addInPort)
		.def("addOutPort", &NodeWrapper::addOutPort)
		.def("setId", &NodeWrapper::setId)
		.def("getParameterNames", n_getParameterNames)
		.def("addParameters", &NodeWrapper::pyAddParameters)
		.def("setIntParameter", &Node::setParameter<int>)
		.def("setDoubleParameter", &Node::setParameter<double>)
		.def("setStringParameter", &Node::setParameter<string>)
		.def("setBoolParameter", &Node::setParameter<bool>)
		.def("setParameter", &Node::setParameter<Flow>)
		;
	python::implicitly_convertible<auto_ptr<NodeWrapper>, auto_ptr<Node> >();
	python::class_<NodeRegistry>("NodeRegistry")
		.def("contains", &NodeRegistry::contains)
		.def("addNativePlugin", &NodeRegistry::addNativePlugin)
		.def("addNodeFactory", nr_addNodeFactory)
		.def("createNode", &NodeRegistry::createNode, python::return_value_policy<python::manage_new_object>())
		.def("getRegisteredNames", nr_getRegisteredNames)
		;
}
