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

#include "orderedworker.h"
#include <boost/foreach.hpp>
#include <memory>
#include <node.h>
#include <imodel.h>
#include <nodeconnection.h>

OrderedWorker::OrderedWorker(OrderedPipeSimulation *sim,
							 Node *last,
							 ptime _time, int _dt) {
	this->sim = sim;
	this->last = last;
	this->time = _time;
	this->dt = _dt;
	out = std::auto_ptr<tqueue<Node *> >(new tqueue<Node *>());
}

void OrderedWorker::run() {
	sim->timestep_before(sim, time);
	Node *current;
	do {
		current = in->dequeue();
		pullPorts(current);
		current->f(time, dt);
		pushPorts(current);
		out->enqueue(current);
	} while (current != last);
	sim->timestep_after(sim, time);
}

void OrderedWorker::pullPorts(Node *sink) {
	BOOST_FOREACH(NodeConnection *con, sim->getModel()->backwardConnection(sink)) {
		con->pull(dt);
	}
}

void OrderedWorker::pushPorts(Node *source) {
	BOOST_FOREACH(NodeConnection *con, sim->getModel()->forwardConnection(source)) {
		con->push(dt);
	}
}
