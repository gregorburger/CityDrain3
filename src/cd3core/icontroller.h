#ifndef ICONTROLLER_H
#define ICONTROLLER_H

class ISimulation;

#include <boost/date_time/posix_time/ptime.hpp>

class IController {
public:
	IController() {

	}

	virtual ~IController() {

	}

	virtual void after_timestep(ISimulation *sim, boost::posix_time::ptime time) = 0;
	virtual void before_timestep(ISimulation *sim, boost::posix_time::ptime time) = 0;
};

#endif // ICONTROLLER_H
