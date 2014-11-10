#include "pythoncontroller.h"
#ifndef PYTHON_DISABLED
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <cd3assert.h>
#include <swigruntime.h>

#define AFTER "timestep_after"
#define BEFORE "timestep_before"

extern "C" {
void init_pycd3(void);
}

PythonController::PythonController(const std::string &path) {
	if (!Py_IsInitialized()) {
		Py_Initialize();
		init_pycd3();
	}

	if (!boost::filesystem::exists(path)) {
		Logger(Error) << "controller " << path << ": file not found";
		return;
	}



	PyObject *main = PyImport_ImportModule("__main__");

	PyObject *main_namespace = PyModule_GetDict(main);
	Py_INCREF(main_namespace);
	Py_DECREF(main);

	PyRun_String("import sys\nsys.path.append('.')\n", Py_file_input, main_namespace, 0);

	PyObject *pycd3_module = PyImport_ImportModule("pycd3");

	PyObject* PyFileObject = PyFile_FromString((char *) path.c_str(), "r");
	PyRun_File(PyFile_AsFile(PyFileObject), path.c_str(), Py_file_input, main_namespace, 0);

	ts_after_cb = PyDict_GetItemString(main_namespace, AFTER);
	ts_before_cb = PyDict_GetItemString(main_namespace, BEFORE);

	Py_DECREF(main_namespace);
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}

PythonController::~PythonController() {
	Py_DECREF(ts_after_cb);
	Py_DECREF(ts_before_cb);
}

void PythonController::after_timestep(ISimulation *sim, boost::posix_time::ptime time) {
	if (ts_after_cb) {
		assert(PyCallable_Check(ts_after_cb));

		swig_type_info *isim_type = SWIG_TypeQuery("ISimulation *");
		cd3assert(isim_type, "could not query swig typeinfo for ISimulation");
		PyObject *py_this = SWIG_NewPointerObj(sim, isim_type, 0);

		PyObject *args = Py_BuildValue("(O)", py_this);

		PyObject_CallObject(ts_after_cb, args);
		if (PyErr_Occurred()) {
			PyErr_Print();
		}
	}
}

void PythonController::before_timestep(ISimulation *sim, boost::posix_time::ptime time) {
	if (ts_before_cb) {
		assert(PyCallable_Check(ts_before_cb));
		PyObject_CallObject(ts_before_cb, NULL);
		if (PyErr_Occurred()) {
			PyErr_Print();
		}
	}
}
#endif //PYTHON_DISABLED
