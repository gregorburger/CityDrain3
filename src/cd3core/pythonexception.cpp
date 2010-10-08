#include <Python.h>
#include "pythonexception.h"

#ifndef PYTHON_DISABLED
std::string to_string(PyObject *o) {
	PyObject *pystr = PyObject_Str(o);
	char *cstr = PyString_AsString(pystr);
	Py_DECREF(pystr);
	return cstr;
}
#endif

PythonException::PythonException() {
#ifndef PYTHON_DISABLED
	if (!Py_IsInitialized() || !PyErr_Occurred()) {
		return;
	}
	PyObject *type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);

	this->type = to_string(type);
	this->value = to_string(value);
	this->traceback = to_string(traceback);
	//PyErr_Print();
#endif
}
