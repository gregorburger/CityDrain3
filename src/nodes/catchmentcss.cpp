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

#include "catchmentcss.h"
#include <flow.h>
#include <flowfuns.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace boost;

CD3_DECLARE_NODE_NAME(CatchmentCSS)

CatchmentCSS::CatchmentCSS() {
	std::string description = 
	        "Catchment CSS block is designed to simulate a combined sewer system on catchment level."
	        "The block copes the major drainage-related processes in an urban area and returns for each"
	        "time step both the current amount and pollutant concentration of the aggregated outflow from"
	        "the catchment. Storm water runoff and water quality aspects are computed here with a set of"
	        "consistently simple conceptual models. The main processes that appear on an urban area in"
	        "this context are\n\n"
	        " * Runoff generation\n"
	        " * Overland flow\n"
	        " * Dry weather flow generation\n"
	        " * Runoff and wasteflow pollution\n\n"
	        "The associated dynamic inputs can be distinguished for inputs that (a) originate from the"
	        "catchment and (b) inputs which originate from upstream and are to be routed through the"
	        "catchment.\n\n"
	        "The modified scheme allows both, feeding of the uppermost block (QI,U) as well a distributed"
	        "feeding of blocks (QI,L). Thus, inputs provided such as the rain intensities (rl) acting on the"
	        "catchment, the dry weather flows generated in the catchment (DWFL) and parasite water"
	        "infiltrating into the sewer system (Qpl) are distributed homogeneously within the catchment."
	        "Flows from upstream of the catchment are all the way routed through and thus are fed to the"
	        "uppermost sub-block. In case of the CSS block an upstream wastewater stream Qe may be"
	        "provided as dynamic input."
	        ;
	setDescription(description);
	
	addInPort(ADD_PARAMETERS(rain_in));
	addInPort(ADD_PARAMETERS(dwf_in));
	addInPort(ADD_PARAMETERS(parasite_in));
	addInPort(ADD_PARAMETERS(q_upstream));

	addOutPort(ADD_PARAMETERS(out));

	addParameter(ADD_PARAMETERS(area))
		.setUnit("ha");
	addParameter(ADD_PARAMETERS(run_off_coeff))
		.setUnit("0-1");
	addParameter(ADD_PARAMETERS(initial_loss))
		.setUnit("mm");
	addParameter(ADD_PARAMETERS(permanent_loss))
		.setUnit("mm/day");
	addParameter(ADD_PARAMETERS(N))
	        .setDescription("Number of sub areas/subreaches");
	addParameter(ADD_PARAMETERS(K))
	        .setDescription("Muskingum parameter describing the time required for a discharge wave travelling"
	                        "through the reach [s]. *K applies to one subreach* and does not cover travelling time"
	                        "for all sub reaches.");
	addParameter(ADD_PARAMETERS(X))
	        .setDescription("Dimensionless weighting factor that relates to the amount of wedge storage [-] in the"
	                        "range of 0 (linear reservoir storage) and 0.5. (Typical value = 0,2).");

	for (size_t i = 0; i < Flow::countUnits(Flow::concentration); i++) {
		rain_concentration.push_back(0.0);
	}

	addParameter(ADD_PARAMETERS(rain_concentration));

	addState(ADD_PARAMETERS(loss_basin));

	area = 208;
	run_off_coeff = 0.2;
	initial_loss = 1.0;
	permanent_loss = 1.25;
	N = 3;
	K = 300;
	X = 0;
}


CatchmentCSS::~CatchmentCSS() {
}

bool CatchmentCSS::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;

	size_t nconcs = Flow::countUnits(Flow::concentration);
	while (rain_concentration.size() < nconcs) {
		rain_concentration.push_back(0.0);
	}

	for (int i = 0; i < N; i++) {
		Flow *f = new Flow();
		addState(str(format("V[%1%]") % i), f);
		V.push_back(f);
	}
	loss_basin.clear();
	return true;
}

void CatchmentCSS::deinit() {
	for (int i = 0; i < N; i++) {
		removeState(str(format("V[%1%]") % i));
		delete V[i];
	}
	V.clear();
}

int CatchmentCSS::f(ptime time, int dt) {
	(void) time;

	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	std::vector<Flow> inputs;
	Flow loss = FlowFuns::catchement_lossmodel(rain_in,
											   &loss_basin,
											   initial_loss,
											   permanent_loss / 86400 * dt,
											   run_off_coeff);
	Flow flow = FlowFuns::catchment_flowmodel(loss, area*10000, dt, rain_concentration);

	inputs.push_back(flow);
	inputs.push_back(dwf_in);
	inputs.push_back(parasite_in);

	Flow rain = FlowFuns::mix(inputs);
	Flow qi = q_upstream;

	for (int i = 0; i < N; i++) {
		qi = FlowFuns::route_catchment(qi, rain, V[i], N, C_x, C_y, dt);
	}
	out = qi;
	return dt;
}

void CatchmentCSS::addMuskParam(int dt) {
	double dt_halve = static_cast<double>(dt) / 2;
	double K_1_min_X = K*(1-X);
	double Cx = (dt_halve - K*X) / (dt_halve + K_1_min_X);
	double Cy = 1.0 / (dt_halve+K_1_min_X);
	musk_param[dt] = std::pair<double, double>(Cx, Cy);
}

void CatchmentCSS::setMuskParam(double *x, double *y, int dt) {
	if (musk_param.find(dt) == musk_param.end()) {
		addMuskParam(dt);
	}
	*x = musk_param[dt].first;
	*y = musk_param[dt].second;
}
