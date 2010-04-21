#include "pynode.h"
#include "../cd3core/node.h"
#include <noderegistry.h>
#include <nodefactory.h>
#include <flow.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <nodefactory.h>
#include "module.h"

using namespace boost;
using namespace boost::assign;

NodeWrapper::NodeWrapper() {
}

NodeWrapper::~NodeWrapper() {
}

void NodeWrapper::setSelf(boost::python::object self) {
	this->self = self;
}

int NodeWrapper::f(ptime time, int dt) {
	updateParameters();
	try {
		if (python::override f = this->get_override("f")) {
			return f(time, dt);
		} else {
			//TODO do something here now reason there is no f method
		}
	} catch(python::error_already_set const &) {
		Logger(Error) << __FILE__ << ":" << __LINE__;
		handle_python_exception();
	}
	return -1;
}

bool NodeWrapper::init(ptime start, ptime stop, int dt) {
	try {
		updateParameters();
		if (python::override init = this->get_override("init"))
			return init(start, stop, dt);
	} catch(python::error_already_set const &) {
		Logger(Error) << __FILE__ << ":" << __LINE__;
		handle_python_exception();
	}
	return true;
}

void NodeWrapper::deinit() {
	try {
		if (python::override deinit = this->get_override("deinit"))
			deinit();
	} catch(python::error_already_set const &) {
		Logger(Error) << __FILE__ << ":" << __LINE__;
		handle_python_exception();
	}
}

const char *NodeWrapper::getClassName() const {
	return class_name;
}

void NodeWrapper::setClassName(string class_name) {
	this->class_name = strdup(class_name.c_str());
}

void NodeWrapper::addInPort(const std::string &name, Flow *inflow) {
	Node::addInPort(name, inflow);
}

void NodeWrapper::addOutPort(const std::string &name, Flow *outflow) {
	Node::addOutPort(name, outflow);
}

typedef pair<string, int> intp;
typedef pair<string, string> stringp;
typedef pair<string, float> floatp;
typedef pair<string, double> doublep;
typedef pair<string, bool> boolp;
typedef pair<string, vector<double> > vecp;
void NodeWrapper::updateParameters() {
	python::dict self_dict = python::extract<python::dict>(self.attr("__dict__"));

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

	BOOST_FOREACH(vecp i, array_params) {
		python::list l;

		BOOST_FOREACH(double d, i.second) {
			l.append(d);
		}

		self_dict[i.first] = l;
	}
}

void addParameters(python::object self) {
	NodeWrapper *node_wrapper = python::extract<NodeWrapper*>(self);

	python::dict param = python::extract<python::dict>(self.attr("__dict__"));

	python::list keys = param.keys();
	for (int i = 0; i < len(keys); i++) {
		string key = python::extract<string>(keys[i]);

		if (key == "self") {
			continue;
		}
		python::extract<int> ix(param[key]);
		if (ix.check()) {
			Logger(Debug) << node_wrapper << "adding int parameter " << key;
			int value = ix;
			node_wrapper->int_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->int_params[key]);
			continue;
		}

		python::extract<string> sx(param[key]);
		if (sx.check()) {
			Logger(Debug) << node_wrapper << "adding string parameter " << key;
			string value = sx;
			node_wrapper->string_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->string_params[key]);
			continue;
		}

		python::extract<double> dx(param[key]);
		if (dx.check()) {
			Logger(Debug) << node_wrapper << "adding double parameter " << key;
			double value = dx;
			node_wrapper->double_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->double_params[key]);
			continue;
		}

		python::extract<float> fx(param[key]);
		if (fx.check()) {
			Logger(Debug) << node_wrapper << "adding float parameter " << key;
			float value = fx;
			node_wrapper->float_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->float_params[key]);
			continue;
		}

		python::extract<Flow> flx(param[key]);
		if (flx.check()) {
			Logger(Debug) << node_wrapper << "adding Flow parameter " << key;
			Flow value = flx;
			node_wrapper->flow_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->flow_params[key]);
			continue;
		}

		python::extract<bool> bx(param[key]);
		if (bx.check()) {
			Logger(Debug) << node_wrapper << "adding bool parameter " << key;
			bool value = bx;
			node_wrapper->bool_params[key] = value;
			node_wrapper->addParameter(key, &node_wrapper->bool_params[key]);
			continue;
		}

		python::extract<python::list> lx(param[key]);
		if (lx.check()) {
			Logger(Debug) << node_wrapper << "adding vector<double> parameter " << key;
			python::list value = lx;
			vector<double> vec_value;
			for (int v = 0; v < python::len(value); v++) {
				double dval = python::extract<double>(value[v]);
				vec_value += dval;
			}
			node_wrapper->array_params[key] = vec_value;
			node_wrapper->addParameter(key, &node_wrapper->array_params[key]);
			continue;
		}
		cout << "unsupported type of param " << key << endl;

	}
}

typedef pair<string, NodeParameter*> param_pair;
static python::list n_getParameterNames(Node &n) {
	python::list l;
	BOOST_FOREACH(param_pair p, n.getParameters()) {
		l.append(p.first);
	}
	return l;
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
		} catch(...) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			handle_python_exception();
		}
		return 0;
	}

	string getNodeName() {
		try {
			return python::call_method<string>(self, "getNodeName");
		} catch(...) {
			Logger(Error) << __FILE__ << ":" << __LINE__;
			handle_python_exception();
		}
		return "";
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
	python::class_<NodeWrapper, auto_ptr<NodeWrapper>, boost::noncopyable>("Node")
		.def("f", python::pure_virtual(&Node::f))
		.def("init", &NodeWrapper::init)
		.def("deinit", &NodeWrapper::deinit)
		.def("addInPort", &NodeWrapper::addInPort)
		.def("addOutPort", &NodeWrapper::addOutPort)
		.def("getParameterNames", n_getParameterNames)
		.def("addParameters", addParameters)
		.def("setIntParameter", &Node::setParameter<int>)
		.def("setDoubleParameter", &Node::setParameter<double>)
		.def("setStringParameter", &Node::setParameter<string>)
		.def("setBoolParameter", &Node::setParameter<bool>)
		.def("setParameter", &Node::setParameter<Flow>)
		;
	//python::implicitly_convertible<auto_ptr<NodeWrapper>, auto_ptr<Node> >();
	python::class_<NodeRegistry>("NodeRegistry")
		.def("contains", &NodeRegistry::contains)
		.def("addNativePlugin", &NodeRegistry::addNativePlugin)
		.def("addNodeFactory", nr_addNodeFactory)
		.def("createNode", &NodeRegistry::createNode, python::return_value_policy<python::manage_new_object>())
		.def("getRegisteredNames", &NodeRegistry::getRegisteredNames)
		;

	python::class_<std::vector<string> >("StringVector")
			.def(python::vector_indexing_suite<std::vector<string> >())
			;
}
