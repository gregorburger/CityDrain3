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

#include "simplemixer.h"

CD3_DECLARE_NODE_NAME(SimpleMixer)

SimpleMixer::SimpleMixer() {
	addInPort(ADD_PARAMETERS(in1));
	addInPort(ADD_PARAMETERS(in2));
	addOutPort(ADD_PARAMETERS(out));
}

bool SimpleMixer::init(ptime start, ptime end, int dt) {
	return true;
}

int SimpleMixer::f(ptime time, int dt) {
	out[0] = in1[0] + in2[0];
	for (int i = 1; i < Flow::size(); i++) {
		out[i] = (in1[1]*in1[0] + in2[i]*in2[0]) / out[0];
	}
	return dt;
}
