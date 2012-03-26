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

#include "impuls.h"

CD3_DECLARE_NODE_NAME(Impuls)

Impuls::Impuls() {
	start_after = 0;
	length = 1200;
	addParameter(ADD_PARAMETERS(start_after))
		.setUnit("s");
	addParameter(ADD_PARAMETERS(length))
		.setUnit("s");
	addParameter(ADD_PARAMETERS(Q))
		.setUnit("m^3/s");
	addOutPort(ADD_PARAMETERS(out));
}

Impuls::~Impuls() {
}

bool Impuls::init(ptime start, ptime end, int dt) {
	if (start_after % dt) {
		Logger(Warning) << "start_after must be a multiple of dt";
		return false;
	}

	if (length % dt) {
		Logger(Warning) << "length must be a multiple of dt";
		return false;
	}

	this->start = start + seconds(start_after);
	this->stop = this->start + seconds(length);
	q.clear();
	return true;
}

int Impuls::f(ptime time, int dt) {
	if (time >= this->start && time < this->stop) {
		q[0] = Q * dt;
		out = q;
	} else {
		out = null;
	}
	return dt;
}
