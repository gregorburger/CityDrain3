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

#include "constsource.h"

#include <flow.h>

CD3_DECLARE_NODE_NAME(ConstSource)

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow))
                .setUnit("m^3/s, g/m^3");
	addOutPort(ADD_PARAMETERS(out));
}

ConstSource::~ConstSource() {
}

int ConstSource::f(ptime time, int dt) {
	(void) time;
	return dt;
}

bool ConstSource::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	out = const_flow;
	out[0] = out[0] * dt;
	return true;
}
