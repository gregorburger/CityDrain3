#ifndef PYTHON_DISABLED
#include <Python.h>
#endif
#include "pythonexception.h"

PythonException::PythonException() {
#ifndef PYTHON_DISABLED
	if (!Py_IsInitialized() || !PyErr_Occurred()) {
		return;
	}
	PyErr_Print();
#endif
}
