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

#include "river.h"

#include <flow.h>
#include <flowfuns.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "reactor.h"

using namespace boost;

CD3_DECLARE_NODE_NAME(River)

River::River() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));

	/* Muskingum Parameters */
	K = 300;
	X = 0.1;
	N = 11;
	addParameter(ADD_PARAMETERS(K))
			.setUnit("K per subreach in s");
	addParameter(ADD_PARAMETERS(X));
	addParameter(ADD_PARAMETERS(N));

	nc = Flow::countUnits(Flow::concentration);
	conc_formula.resize(nc);
	formula_name.resize(nc);

	addParameter(ADD_PARAMETERS(constants));

	nstep = 10;

	addParameter(ADD_PARAMETERS(nstep)).setUnit("-");
	addParameter(ADD_PARAMETERS(init_v)).setUnit("[-]");

	for (int c = 0; c < nc; ++c) {
		std::string cname = Flow::getUnitNames(Flow::concentration)[c];
		formula_name[c] = cname + "_formula";
		conc_formula[c] = std::string("0");
		addParameter(formula_name[c], &conc_formula[c]);
	}
}

River::~River() {
}

void River::deinit() {
	for (int i = 0; i < N; i++) {
		Flow *f = getState<Flow>(str(format("V[%1%]") % i));
		removeState(str(format("V[%1%]") % i));
		delete reactors[i];
		delete f;
	}
	reactors.clear();
	V.clear();
}

bool River::init(ptime start, ptime end, int dt) {
	(void) end;
	for (int i = 0; i < N; i++) {
		V.push_back(new Flow());
		addState(str(format("V[%1%]") % i), V[i]);
		reactors.push_back(new Reactor(*V[i], true));   // Reactor block defined with variable volume (true) - pointer to V[i]
		reactors[i]->nstep = nstep;
		reactors[i]->init_v = init_v;
		if (!reactors[i]->init(dt, constants, conc_formula)) {
			return false;
		}
	}

	addMuskParam(dt);
	return true;
}


int River::f(ptime time, int dt) {
	(void) time;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);
	Flow tmp = in;

	for (int i = 0; i < N; i++) {

		// Muskingum routing
		// tmp = Vector inflow
		// *(V[i])= Vector subreach
		// Calculation of hydraulics with timestep dt >> C_x and C_y
		double inqe = tmp[0];
		double volqe = (*(V[i]))[0];
		double outqe = (inqe / dt * C_x + volqe * C_y) * dt;
		double newvolqe = (inqe - outqe) + volqe;
		double dvolqe = (inqe - outqe);

		// Reactor with variable Volume
		reactors[i]->react_dV(tmp, dvolqe,dt);

		// Write Flow and Concentration back in tmp for next subreach inflow
		tmp = (*(V[i]));
		tmp[0] = outqe;

		// Store newvolume in V[i] - just safety/should be OK anyway
		(*(V[i]))[0]=newvolqe;

	}
	out = tmp;
	return dt;
}

void River::addMuskParam(int dt) {
	double dt_halve = static_cast<double>(dt) / 2;
	double K_1_min_X = K*(1-X);
	double Cx = (dt_halve - K*X) / (dt_halve + K_1_min_X);
	double Cy = 1.0 / (dt_halve+K_1_min_X);
	musk_param[dt] = std::pair<double, double>(Cx, Cy);
}

void River::setMuskParam(double *x, double *y, int dt) {
	if (musk_param.find(dt) == musk_param.end()) {
		addMuskParam(dt);
	}
	*x = musk_param[dt].first;
	*y = musk_param[dt].second;
}
