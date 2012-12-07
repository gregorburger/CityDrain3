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

#include "timestephandler.h"
#include <simulation.h>

TimeStepHandler::TimeStepHandler() {
	old_progress = -1;
}

TimeStepHandler::~TimeStepHandler() {
}

void TimeStepHandler::after(ISimulation *sim, ptime t) {
	SimulationParameters sp = sim->getSimulationParameters();
	long totsecs = time_period(sp.start + seconds(sp.dt), sp.stop).length().total_seconds();
	long cursecs = time_period(sp.start + seconds(sp.dt), t).length().total_seconds();
	int cur_prog = qRound(100.0 / totsecs * cursecs);
	if (old_progress != cur_prog) {
		old_progress = cur_prog;
		Q_EMIT(progress(cur_prog));
	}
	if (sp.stop == t) {
		Q_EMIT(finished());
	}
}
