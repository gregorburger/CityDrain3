#ifndef PYTHONCONTROLLER_H
#define PYTHONCONTROLLER_H

#ifndef PYTHON_DISABLED

#include "icontroller.h"
#include <Python.h>

#include <cd3globals.h>

class CD3_PUBLIC PythonController : public IController {
public:
	PythonController(const std::string &script_path);
	virtual ~PythonController();

	void after_timestep(ISimulation *sim, boost::posix_time::ptime time);
	void before_timestep(ISimulation *sim, boost::posix_time::ptime time);
private:
	PyObject *ts_after_cb, *ts_before_cb;

};

#endif // PYTHON_DISABLED

#endif // PYTHONCONTROLLER_H
