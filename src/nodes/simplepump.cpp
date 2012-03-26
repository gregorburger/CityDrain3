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

#include "simplepump.h"
#include <flowfuns.h>
#include <boost/assign.hpp>

CD3_DECLARE_NODE_NAME(SimplePump)


SimplePump::SimplePump() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out_w));
	addOutPort(ADD_PARAMETERS(out_p));
	Qp = 0.005;
	Voff = 0;
	Von = 1.5;
	Vmax = 1.5;
	addParameter(ADD_PARAMETERS(Von))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Voff))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Qp))
		.setUnit("m^3/s");
	addParameter(ADD_PARAMETERS(Vmax))
		.setUnit("m^3");
	addState(ADD_PARAMETERS(on));
}

SimplePump::~SimplePump() {

}

using namespace boost::assign;

int SimplePump::f(ptime time, int dt) {
	(void) time;

	//volume[0] += in[0] * dt;
	std::vector<Flow> tomix;
	tomix += volume, in;
	volume = FlowFuns::mix(tomix);
	if (volume[0] > Von || on) {
		on = true;
	}
	out_p = volume; //assign concentrations
	out_w = volume;
	if (on) {
		double volume_after_pump = std::max(Voff, volume[0] - Qpp);
		out_p[0] = (volume[0] - volume_after_pump);
		volume[0] = volume_after_pump;
		out_w[0] = std::max(0.0, volume[0] - Vmax);
		volume[0] -= out_w[0];
	} else {
		out_p[0] = 0.0;
		out_w[0] = 0.0;
	}
	if (volume[0] <= Voff) {
		on = false;
	}
	return dt;
}

bool SimplePump::init(ptime start, ptime end, int dt) {
	if (Voff >= Von) {
		Logger(Warning) << "Von must be smaller than Voff";
		return false;
	}

	if (Von > Vmax) {
		Logger(Warning) << "Von must be smaller than Vmax";
		return false;
	}
	volume.clear();
	Qpp = Qp * dt;
	on = false;
	return true;
}

void SimplePump::deinit() {

}
