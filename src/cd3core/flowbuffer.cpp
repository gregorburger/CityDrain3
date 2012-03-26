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

#include "flowbuffer.h"

#include <flow.h>
#include <flowfuns.h>
#include <cd3assert.h>

#include <vector>
#include <boost/foreach.hpp>

FlowBuffer::FlowBuffer() {
	buffereddt = 0;
}

FlowBuffer::~FlowBuffer() {
}

void FlowBuffer::put(const Flow &f, int dt) {
	cd3assert(dt > 0, "cannot buffer negative dt flow");
	//cd3assert(f, "cannot buffer null flow");
	buffer.push(buf_item(f, dt));
	buffereddt += dt;
}

Flow FlowBuffer::take(int dt) {
	cd3assert(dt <= buffereddt, "cannot take more flow than available");
	//cd3assert(buffer->getUnitNames(CalculationUnit::flow).size() == 1, "no more than onw flow allowed");

	if (buffer.size() == 1) {
		cd3assert(buffereddt == dt, "TODO implement rest mixing in flow buffer");
		Flow single = buffer.front().first;
		buffer.pop();
		buffereddt -= dt;
		return single;
	}

	std::vector<Flow> tmp;
	int taken_dt = 0;

	while (taken_dt < dt) {
		buf_item bitem = buffer.front();
		taken_dt += bitem.second;
		tmp.push_back(bitem.first);
		buffer.pop();
	}
	//std::cout << taken_dt << " - " << dt << std::endl;
	cd3assert(taken_dt == dt, "TODO implement rest mixing in flow buffer");

	Flow mixed = FlowFuns::mix(tmp);
	buffereddt -= taken_dt;

	/*BOOST_FOREACH(const Flow *f, tmp) {
		delete f;
	}*/

	//delete all in tmp

	return mixed;
}

int FlowBuffer::buffered() const {
	return buffereddt;
}
