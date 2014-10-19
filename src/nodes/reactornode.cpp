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

#include "reactornode.h"
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <QString>
#include <QStringList>
#include "reactor.h"

const char *ReactorNode::class_name = "Reactor"; 
const char *ReactorNode::getClassName() const { return ReactorNode::class_name; }

ReactorNode::ReactorNode() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));

	nc = Flow::countUnits(Flow::concentration);
	conc_formula.resize(nc);
	formula_name.resize(nc);

	reactor = new Reactor(volume);

	addParameter(ADD_PARAMETERS(constants));

	addParameter("reactor_volume", &reactor->reactor_volume).setUnit("m^3");
	addParameter("nstep", &reactor->nstep).setUnit("-");
	addParameter("init_v", &reactor->init_v).setUnit("[-]");

	for (int c = 0; c < nc; ++c) {
		std::string cname = Flow::getUnitNames(Flow::concentration)[c];
		formula_name[c] = cname + "_formula";
		conc_formula[c] = std::string("0");
		addParameter(formula_name[c], &conc_formula[c]);
	}
}


ReactorNode::~ReactorNode() {
	delete reactor;
}

bool ReactorNode::init(ptime start, ptime end, int dt) {
	return reactor->init(dt, constants, conc_formula);
}


int ReactorNode::f(ptime time, int dt) {
	(void)
			reactor->react(in, dt);
	// Ausgabe
	for (int i=0; i < nc; i++){
		out[i+1] = volume[i+1];
	}
	out[0] = in[0];

	return dt;
}
