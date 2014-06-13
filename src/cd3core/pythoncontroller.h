#ifndef PYTHONCONTROLLER_H
#define PYTHONCONTROLLER_H

#include "icontroller.h"
#include <Python.h>

class PythonController : public IController {
public:
	PythonController(const std::string &script_path);
	virtual ~PythonController();

	void after_timestep(ISimulation *sim, boost::posix_time::ptime time);
	void before_timestep(ISimulation *sim, boost::posix_time::ptime time);
private:
	PyObject *ts_after_cb, *ts_before_cb;

};

#endif // PYTHONCONTROLLER_H
