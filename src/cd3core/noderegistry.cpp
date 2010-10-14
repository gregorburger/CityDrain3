#ifndef PYTHON_DISABLED
#include <Python.h>
#include <swigruntime.h>
#include <pythonexception.h>
#endif

#include "noderegistry.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <QLibrary>

using namespace boost;

#include "nodefactory.h"
#include <cd3assert.h>

std::vector<std::string> NodeRegistry::python_paths = std::vector<std::string>();
void NodeRegistry::addToPythonPath(std::string p) {
	NodeRegistry::python_paths.push_back(p);
}

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
		PyObject *main = PyImport_ImportModule("__main__");
		main_namespace = PyModule_GetDict(main);
		Py_DECREF(main);
	}

	PyRun_String("import sys\nsys.path.append('.')\n", Py_file_input, main_namespace, 0);

	BOOST_FOREACH(std::string path, python_paths) {
		std::string app_pp_cmd = "sys.path.append('"+path+"')\n";
		PyRun_String(app_pp_cmd.c_str(), Py_file_input, main_namespace, 0);
	}

	FILE *test_py = fopen(script.c_str(), "r");
	PyRun_File(test_py, script.c_str(), Py_file_input, main_namespace, 0);

	if (PyErr_Occurred())
		PyErr_Print();

	PyObject *callback = 0;
	PyObject *register_cb_name = PyString_FromString("register");

	//check for a 'register' callback in script or else call 'pycd3.registerAllNodes'
	if (PyDict_Contains(main_namespace, register_cb_name)) {
		callback = PyDict_GetItemString(main_namespace, "register");
		Logger(Debug) << "calling register in script";
	} else {
		//load pycd3 module an get 'registerAllCallback'
		PyObject *pycd3_module = PyImport_ImportModule("pycd3");
		if (PyErr_Occurred()) {
			PyErr_Print();
			return;
		}
		PyObject *pycd3_dict = PyModule_GetDict(pycd3_module);
		Py_XDECREF(pycd3_module);
		callback = PyDict_GetItemString(pycd3_dict, "registerAllNodes");
		Logger(Debug) << "calling internal registerAllNodes";
	}
	Py_XDECREF(register_cb_name);

	//prepare args and call 'callback'
	swig_type_info *node_reg = SWIG_TypeQuery("NodeRegistry *");
	cd3assert(node_reg, "could not query swig typeinfo for NodeRegistry");
	PyObject *py_this = SWIG_NewPointerObj(this, node_reg, 0);

	PyObject *args = Py_BuildValue("(O)", py_this);

	PyObject *res = PyObject_Call(callback, args, Py_None);
	Py_XDECREF(res);
	Py_XDECREF(py_this);
	Py_XDECREF(args);
	if (PyErr_Occurred()) {
		Logger(Error) << "error in python register function";
		throw new PythonException();
	}
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
