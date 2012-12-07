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

#include "splitter.h"
#include <flow.h>
#include <flowfuns.h>

CD3_DECLARE_NODE_NAME(Splitter)

Splitter::Splitter() {
	in = new Flow();
	out1 = new Flow();
	out2 = new Flow();

	addInPort(ADD_PARAMETERS_P(in));
	addOutPort(ADD_PARAMETERS_P(out1));
	addOutPort(ADD_PARAMETERS_P(out2));

	ratio = 0.5;
	addParameter(ADD_PARAMETERS(ratio));
}

Splitter::~Splitter() {
	delete out2;
	delete out1;
	delete in;
}

bool Splitter::init(ptime start, ptime end, int dt) {
	return (ratio >= 0 && ratio <= 1.0);
}

int Splitter::f(ptime time, int dt) {
	(void) time;

	std::pair<Flow, Flow> splitted = FlowFuns::split(*in, ratio);
	*out1 = splitted.first;
	*out2 = splitted.second;
	return dt;
}
