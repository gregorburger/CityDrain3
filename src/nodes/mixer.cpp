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

#include "mixer.h"

#include <flow.h>
#include <flowfuns.h>
#include <boost/foreach.hpp>
#include <sstream>

CD3_DECLARE_NODE_NAME(Mixer)

Mixer::Mixer() {
	num_inputs = 2;
	addParameter(ADD_PARAMETERS(num_inputs));
	addOutPort(ADD_PARAMETERS(out));
}

Mixer::~Mixer() {
}

void Mixer::deinit() {
	BOOST_FOREACH(std::string name, input_names) {
		removeInPort(name);
	}
	BOOST_FOREACH(Flow *f, inputs) {
		delete f;
	}
	inputs.clear();
	input_names.clear();
}

bool Mixer::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	for (int i = 0; i < num_inputs; i++) {
		Flow *tmp = new Flow();
		std::ostringstream name;
		name << "in_" << i;
		addInPort(name.str(), tmp);
		inputs.push_back(tmp);
		input_names.push_back(name.str());
	}
	return true;
}

int Mixer::f(ptime time, int dt) {
	(void) time;
	out = FlowFuns::mix(inputs);
	return dt;
}

