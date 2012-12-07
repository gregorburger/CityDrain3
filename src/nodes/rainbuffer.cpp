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

#include "rainbuffer.h"
#include <assert.h>

RainBuffer::RainBuffer() {
	clear();
}

void RainBuffer::put(int dt, double value) {
	buffer.push_back(rain_event(dt, value));
	avail.push_back(dt);
	this->dt += dt;
}

int RainBuffer::available() const {
	return dt;
}

double RainBuffer::take(int dt) {
	assert(dt > 0);
	double rain = 0.0;
	while (dt > 0) {
		if (dt >= avail.front()) {
			rain += 1.0 / buffer.front().first * avail.front() * buffer.front().second;
			dt -= avail.front();
			this->dt -= avail.front();
			avail.pop_front();
			buffer.pop_front();
			continue;
		}
		rain += 1.0 / buffer.front().first * dt * buffer.front().second;
		avail.front() -= dt;
		this->dt -= dt;
		dt = 0;
	}
//	assert(dt == 0);
	return rain;
}

void RainBuffer::clear() {
	dt = 0;
	avail.clear();
	buffer.clear();
}
