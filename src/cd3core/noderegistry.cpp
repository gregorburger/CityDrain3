/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#ifndef PYTHON_DISABLED
#define SWIG_PYTHON_THREADS
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

#ifndef PYTHON_DISABLED
std::vector<std::string> NodeRegistry::python_paths = std::vector<std::string>();
PyObject *NodeRegistry::main_namespace = 0;
void NodeRegistry::addToPythonPath(std::string p) {
	NodeRegistry::python_paths.push_back(p);
}
#endif

extern "C" void registerNodes(NodeRegistry *registry);

NodeRegistry::NodeRegistry() {
	Logger(Debug) << "registering builtin Nodes";
	registerNodes(this);
}

typedef std::pair<std::string, INodeFactory *> rn_ptype;

NodeRegistry::~NodeRegistry() {
	BOOST_FOREACH(rn_ptype t, registered_nodes) {
		delete t.second;
	}
}

bool NodeRegistry::addNodeFactory(INodeFactory *factory) {
    if (contains(factory->getNodeName())) {
        Logger(Warning) << str(format("NodeFactory already registered for that name: %1%") %factory->getNodeName());
        return false;
    }
	registered_nodes[factory->getNodeName()] = factory;
	return true;
}

void NodeRegistry::addNativePlugin(const std::string &plugin_path) {
	QLibrary l(QString::fromStdString(plugin_path));
	bool loaded = l.load();
	if (!loaded && plugin_path == "nodes") {
		Logger(Warning) << "ignoring native plugin \"nodes\" because it is loaded as builtin";
		return;
	}
	if (!loaded) {
		Logger(Error) << str(format("could not load plugin %1%: %2%")
							  % plugin_path
							  % l.errorString().toStdString());
		return;
	}
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
        Py_INCREF(main_namespace);
        Py_DECREF(main);

        PyRun_String("import sys\nsys.path.append('.')\n", Py_file_input, main_namespace, 0);

        PyObject *pycd3_module = PyImport_ImportModule("pycd3");
        if (PyErr_Occurred()) {
            PyErr_Print();
            return;
        }
        PyObject *pycd3_dict = PyModule_GetDict(pycd3_module);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        Py_XDECREF(pycd3_module);
        PyObject *callback = PyDict_GetItemString(pycd3_dict, "install_redirector");
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyObject *res = PyObject_Call(callback, Py_None, Py_None);
        if (PyErr_Occurred()) {
            PyErr_Print();
            return;
        }
        Py_XDECREF(res);

        SWIG_PYTHON_INITIALIZE_THREADS;
        PyThreadState *pts = PyGILState_GetThisThreadState();
        PyEval_ReleaseThread(pts);
    }

    if(!main_namespace) {
        SWIG_PYTHON_THREAD_BEGIN_BLOCK;
        init_pycd3();
        PyObject *main = PyImport_ImportModule("__main__");
        main_namespace = PyModule_GetDict(main);
        Py_DECREF(main);
    }

    SWIG_PYTHON_THREAD_BEGIN_BLOCK;

    if (PyErr_Occurred()) {
        Logger(Error) << "unknown python error" << script;
        throw PythonException();
    }

	PyRun_String("import sys\nsys.path.append('.')\n", Py_file_input, main_namespace, 0);

    if (PyErr_Occurred()) {
        Logger(Error) << "" << script;
        throw PythonException();
    }

	BOOST_FOREACH(std::string path, python_paths) {
		std::string app_pp_cmd = "sys.path.append('"+path+"')\n";
		Logger(Debug) << "adding" << path << "to pyhton sys.path";
		PyRun_String(app_pp_cmd.c_str(), Py_file_input, main_namespace, 0);
        if (PyErr_Occurred()) {
            Logger(Error) << "error adding path to sys.path" << script;
            throw PythonException();
        }
	}
	
	PyObject* PyFileObject = PyFile_FromString((char *) script.c_str(), "r");
	PyRun_File(PyFile_AsFile(PyFileObject), script.c_str(), Py_file_input, main_namespace, 0);

	if (PyErr_Occurred()) {
		Logger(Error) << "error loading python script" << script;
		throw PythonException();
	}

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
			Logger(Error) << "error importint pycd3 module";
			throw PythonException();
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
		throw PythonException();
	}
	Logger(Debug) << "successfully loaded python script" << script;
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
