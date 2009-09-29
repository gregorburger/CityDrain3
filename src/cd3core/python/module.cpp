#include <Python.h>
#include "module.h"
#include "../node.h"
#include <flow.h>
#include <noderegistry.h>
#include "pythonnodefactory.h"

#include <boost/python.hpp>

using namespace boost::python;

struct NodeWrapper : Node, wrapper<Node> {
public:
	//virtual ~NodeWrapper(){};
	int f(int time, int dt) {
		return this->get_override("f")(time, dt);
	}

	void init(int start, int stop, int dt) {
		this->get_override("init")(start, stop, dt);
	}

	const char *getClassName() const {
		return 0;
	}
};


BOOST_PYTHON_MODULE(cd3) {
	class_<NodeWrapper, boost::noncopyable>("Node")
			.def("f", pure_virtual(&Node::f))
			.def("init", pure_virtual(&Node::init))
			;
	class_<Flow>("Flow")
			;
}

void registerPythonNodes(NodeRegistry *registry, const string &module) {
	Py_Initialize();

	initcd3();

	try {
		object main_module = import("__main__");
		object main_namespace = main_module.attr("__dict__");
		object result = exec(
				"import cd3\n"
				"__import__('cdtest', None, None, [], 1)\n"
				"clss = cd3.Node.__subclasses__()\n"
				, main_namespace, main_namespace);
		object clss = main_namespace["clss"];
		cout << len(clss) << endl;
		for (int i = 0; i < len(clss); i++) {
			string cname = extract<string>(clss[i].attr("__name__"));
			cout << cname << endl;
			registry->addNodeFactory(new PythonNodeFactory(clss[i], cname));
		}
	} catch(error_already_set const &) {
		PyErr_Print();
	}

}
