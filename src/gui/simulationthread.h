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

#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>
#include "timestephandler.h"
#include <boost/signals2.hpp>
#include <pythonexception.h>

class ISimulation;

class SimulationThread : public QThread
{
public:
	SimulationThread();
	virtual ~SimulationThread();
	void setSimulation(ISimulation *simulation) { this->simulation = simulation; }
	void run();
	TimeStepHandler *handler;
	ISimulation *getSimulation() const { return simulation; }
	inline bool hasFailed() const { return failed; }
	PythonException exception;
private:
	boost::signals2::connection c;
	ISimulation *simulation;
	bool failed;
};

#endif // SIMULATIONTHREAD_H
