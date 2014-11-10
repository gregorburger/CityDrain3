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

#include "simulationthread.h"
#include <simulation.h>
#include <imodel.h>
#include <boost/bind.hpp>
#include <pythonexception.h>
#include <logger.h>

SimulationThread::SimulationThread()
	: handler(new TimeStepHandler()), simulation(0), failed(false) {
}

SimulationThread::~SimulationThread() {
	c.disconnect();
	delete handler;
}

void SimulationThread::run() {
	Q_ASSERT(simulation);
	failed = false;
	SimulationParameters sp = simulation->getSimulationParameters();
	c = simulation->timestep_after.connect(boost::bind(&TimeStepHandler::after, handler, _1, _2));
	try {
		simulation->getModel()->deinitNodes();
		simulation->getModel()->initNodes(sp);//TODO check for uninited nodes here
		simulation->start(sp.start);
	} catch (PythonException e) {
		exception = e;
		failed = true;
	}
}
