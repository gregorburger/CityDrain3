#include "module.h"
#include <noderegistry.h>
#include "pythonnodefactory.h"

#include "pymodel.h"
#include "pynode.h"
#include "pyflow.h"

#include <boost/python.hpp>

using namespace boost::python;


void init() {
	Log::init();
}

BOOST_PYTHON_MODULE(pycd3) {
	wrap_node();
	wrap_flow();
	wrap_model();
	def("init", ::init);
}

struct PythonEnvPriv {
	object main_module, main_namespace;
};

PythonEnv *PythonEnv::instance = 0;

PythonEnv::PythonEnv() {
	priv = new PythonEnvPriv();
	Py_Initialize();
	initpycd3();
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
				"import pycd3\n"
				"__import__('cdtest', None, None, [], 1)\n"
				"clss = pycd3.Node.__subclasses__()\n"
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
