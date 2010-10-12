#ifndef PYTHON_DISABLED
#include <Python.h>
#include <swigruntime.h>
#endif

#include "noderegistry.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <QLibrary>

using namespace boost;

#include "nodefactory.h"
#include <cd3assert.h>

std::vector<std::string> NodeRegistry::python_paths;

NodeRegistry::NodeRegistry() {
}

typedef std::pair<std::string, INodeFactory *> rn_ptype;

NodeRegistry::~NodeRegistry() {
	BOOST_FOREACH(rn_ptype t, registered_nodes) {
		delete t.second;
	}
}

bool NodeRegistry::addNodeFactory(INodeFactory *factory) {
	cd3assert(!contains(factory->getNodeName()), str(format("NodeFactory already registered for that name: %1%") %factory->getNodeName()));
	registered_nodes[factory->getNodeName()] = factory;
	return true;
}

void NodeRegistry::addNativePlugin(const std::string &plugin_path) {
	QLibrary l(QString::fromStdString(plugin_path));
	bool loaded = l.load();
	cd3assert(loaded, str(format("could not load plugin %1%: %2%")
						  % plugin_path
						  % l.errorString().toStdString()));
	regNodeFunProto regNodeFun = (regNodeFunProto) l.resolve("registerNodes");
	if (regNodeFun) {
		regNodeFun(this);
	} else {
		Logger(Warning) << plugin_path << " has no node register hook";
	}
}

#ifndef PYTHON_DISABLED
extern "C" {
void init_pycd3(void);
}

void NodeRegistry::addPythonPlugin(const std::string &script) {
	if (!Py_IsInitialized()) {
		Py_Initialize();
		init_pycd3();
	}
	PyObject *main = PyImport_ImportModule("__main__");
	PyObject *main_namespace = PyModule_GetDict(main);

	PyRun_String("import sys\nsys.path.append('.')\n", Py_file_input, main_namespace, 0);

	BOOST_FOREACH(std::string path, python_paths) {
		std::string app_pp_cmd = "sys.path.append('"+path+"')\n";
		PyRun_String(app_pp_cmd.c_str(), Py_file_input, main_namespace, 0);
	}

	FILE *test_py = fopen(script.c_str(), "r");
	PyRun_File(test_py, script.c_str(), Py_file_input, main_namespace, 0);

	if (PyErr_Occurred())
		PyErr_Print();

	PyObject *register_cb_name = PyString_FromString("register");
	if (PyDict_Contains(main_namespace, register_cb_name)) {
		//cout << "contains a register cb"  << endl;
		PyObject *register_cb = PyDict_GetItem(main_namespace, register_cb_name);

		swig_type_info *node_reg = SWIG_TypeQuery("NodeRegistry *");
		cd3assert(node_reg, "could not query swig typeinfo for NodeRegistry");
		PyObject *py_this = SWIG_NewPointerObj(this, node_reg, 0);

		PyObject *args = Py_BuildValue("(O)", py_this);

		PyObject *res = PyObject_Call(register_cb, args, Py_None);
		if (res == 0) {
			PyErr_Print();
		} else {
			//Py_DECREF(res);
		}

		Py_DECREF(py_this);
		Py_DECREF(args);

		if (PyErr_Occurred())
			PyErr_Print();
	} else {
		Logger(Error) << "script has no register callback";
	}
	Py_DECREF(register_cb_name);
	Py_DECREF(main);
}
#endif

typedef std::pair<std::string, INodeFactory *> snf;
std::vector<std::string> NodeRegistry::getRegisteredNames() const {
	std::vector<std::string> names;

	BOOST_FOREACH(snf item, registered_nodes) {
		names.push_back(item.first);
	}

	return names;
}

typedef std::map<std::string, std::vector<std::string> > names_and_sources;
names_and_sources
NodeRegistry::getRegisteredNamesAndSources() const {
	names_and_sources n_and_s;
	BOOST_FOREACH(snf item, registered_nodes) {
		n_and_s[item.second->getSource()].push_back(item.first);
	}
	return n_and_s;
}

Node *NodeRegistry::createNode(const std::string &name) const {
	cd3assert(contains(name),
			  str(format("no such node class registered: %1%") % name));
	return registered_nodes.find(name)->second->createNode();
}

bool NodeRegistry::contains(const std::string &name) const {
	return registered_nodes.find(name) != registered_nodes.end();
}
