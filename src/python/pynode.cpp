#include "pynode.h"
#include "../cd3core/node.h"
#include <flow.h>
#include <boost/python.hpp>
#include <boost/foreach.hpp>

using namespace boost;

struct NodeWrapper : Node, python::wrapper<Node> {
	NodeWrapper(PyObject *_self) : self(_self) {
		cout << "NodeWrapper()" << endl;
	}

	~NodeWrapper() {
		cout << "NodeWrapper() cya!" << endl;
	}

	int f(int time, int dt) {
		try {
			return python::call_method<int>(self, "f", time, dt);
		} catch(python::error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
		}
	}

	void pushInStates() {

	}

	void pullOutStates() {

	}

	typedef pair<string, int> intp;
	void init(int start, int stop, int dt) {
		try {
			python::object s = python::object(python::handle<>(python::borrowed(self)));
			python::dict param = python::extract<python::dict>(s.attr("__dict__"));
			BOOST_FOREACH(intp i, int_params) {
				param[i.first] = i.second;
			}
			python::call_method<void>(self, "init", start, stop, dt);

		} catch(python::error_already_set const &) {
			cerr << __FILE__ << ":" << __LINE__ << endl;
			PyErr_Print();
			abort();
		}
	}

	const char *getClassName() const {
		python::object o(this);
		const char *name = python::extract<const char*>(o.attr("__name__"));
		return name;
	}

	void addInPort(const std::string &name, Flow *inflow) {
		Node::addInPort(name, inflow);
	}

	void addOutPort(const std::string &name, Flow *outflow) {
		Node::addOutPort(name, outflow);
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
				int value = ix;
				int_params[key] = value;
				addParameter(key, &int_params[key]);
				continue;
			}

			python::extract<string> sx(param[key]);
			if (sx.check()) {
				string value = sx;
				string_params[key] = value;
				addParameter(key, &string_params[key]);
				continue;
			}

			python::extract<double> dx(param[key]);
			if (dx.check()) {
				double value = dx;
				double_params[key] = value;
				addParameter(key, &double_params[key]);
				continue;
			}
			cout << "unsupported type of param " << key << endl;
		}
	}

	map<string, int> int_params;
	map<string, string> string_params;
	map<string, double> double_params;
	PyObject *self;
};

static void test_node(shared_ptr<NodeWrapper> n) {
	n->setParameter("x", 20);
	n->init(0, 900, 300);
	n->f(0, 300);
	n->f(300, 300);
}

void wrap_node() {
	python::class_<Node, shared_ptr<NodeWrapper>, boost::noncopyable>("Node")
		.def("f", &NodeWrapper::f)
		.def("init", &NodeWrapper::init)
		.def("addInPort", &NodeWrapper::addInPort)
		.def("addOutPort", &NodeWrapper::addOutPort)
		.def("addParameters", &NodeWrapper::pyAddParameters)
		;
	python::implicitly_convertible<shared_ptr<NodeWrapper>, shared_ptr<Node> >();
	python::def("test_node", test_node);
}
