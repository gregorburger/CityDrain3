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

#include "catchmentbwmcss.h"
#include <flow.h>
#include <flowfuns.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace boost;

inline bool fuzzyIsNull(double d)
{
    return fabs(d) <= 0.000000000001;
}

CD3_DECLARE_NODE_NAME(CatchmentBWMCSS)

CatchmentBWMCSS::CatchmentBWMCSS() {
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
	addParameter(ADD_PARAMETERS(N));
	addParameter(ADD_PARAMETERS(K));
	addParameter(ADD_PARAMETERS(X));

	mass_init.resize(Flow::countUnits(Flow::concentration), 1.0);
	mass.resize(Flow::countUnits(Flow::concentration), 0);
	mass_soluted.resize(Flow::countUnits(Flow::concentration), 0);
	buildup_a.resize(Flow::countUnits(Flow::concentration), 1.0);
	buildup_b.resize(Flow::countUnits(Flow::concentration), 1.0);
	washoff_k.resize(Flow::countUnits(Flow::concentration), 1.0);
	washoff_w.resize(Flow::countUnits(Flow::concentration), 1.0);

	addParameter(ADD_PARAMETERS(mass_init))
			.setUnit("kg");
	addParameter(ADD_PARAMETERS(buildup_a))
			.setUnit("kg/ha/day");
	addParameter(ADD_PARAMETERS(buildup_b))
			.setUnit("1/day");
	addParameter(ADD_PARAMETERS(washoff_k))
			.setUnit("1/mm");
	addParameter(ADD_PARAMETERS(washoff_w));

	addState(ADD_PARAMETERS(mass));
	addState(ADD_PARAMETERS(mass_soluted));

	addState(ADD_PARAMETERS(loss_basin));


	area = 208;
	run_off_coeff = 0.2;
	initial_loss = 1.0;
	permanent_loss = 1.25;
	N = 3;
	K = 300;
	X = 0;
}


CatchmentBWMCSS::~CatchmentBWMCSS() {
}

bool CatchmentBWMCSS::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;

	size_t nconcs = Flow::countUnits(Flow::concentration);

	for (size_t i = 0; i < nconcs; i++){
		mass[i] = mass_init[i];
		mass_soluted[i] = 0;
	}
	for (int i = 0; i < N; i++) {
		Flow *f = new Flow();
		addState(str(format("V[%1%]") % i), f);
		V.push_back(f);
	}
	loss_basin.clear();
	return true;
}

void CatchmentBWMCSS::deinit() {
	for (int i = 0; i < N; i++) {
		removeState(str(format("V[%1%]") % i));
		delete V[i];
	}
	V.clear();
}

int CatchmentBWMCSS::f(ptime time, int dt) {
	(void) time;

	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	std::vector<Flow> inputs;
	Flow loss = FlowFuns::catchement_lossmodel(rain_in,
											   &loss_basin,
											   initial_loss,
											   permanent_loss / 86400 * dt,
											   run_off_coeff);
	std::vector<double> rain_concentration;
	for(size_t i =1; i<rain_in.size(); i++)
			rain_concentration.push_back(0.0);


	Flow flow = FlowFuns::catchment_flowmodel(loss, area*10000, dt, rain_concentration);


	if(fuzzyIsNull(rain_in[0]))     //build-up
	{

		//std::cout << "I = 0 ";
		for(size_t i=0; i<rain_concentration.size(); i++){
			mass[i]= buildup_a[i]*area/buildup_b[i]+exp(-buildup_b[i]*86400.*dt)*(mass[i]-buildup_a[i]*area/buildup_b[i]);
		}
	}
	else                            //wah-off
	{
		std::vector<double>old_mass(mass);
		for(size_t i=0; i<rain_concentration.size(); i++){
			double cons = -washoff_k[i]*pow(rain_in[0], washoff_w[i]);
			mass[i] = exp(cons*dt)*mass[i];
			mass_soluted[i]+=old_mass[i]-mass[i];
		}
	}

	for(size_t i=0; i<Flow::size()-1; i++){
		if(flow[0]!=0){
			rain_concentration[i]+=mass_soluted[i]*1000./flow[0];
			mass_soluted[i]=0;
		}
		flow[i+1]=rain_concentration[i];
	}


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

void CatchmentBWMCSS::addMuskParam(int dt) {
	double dt_halve = static_cast<double>(dt) / 2;
	double K_1_min_X = K*(1-X);
	double Cx = (dt_halve - K*X) / (dt_halve + K_1_min_X);
	double Cy = 1.0 / (dt_halve+K_1_min_X);
	musk_param[dt] = std::pair<double, double>(Cx, Cy);
}

void CatchmentBWMCSS::setMuskParam(double *x, double *y, int dt) {
	if (musk_param.find(dt) == musk_param.end()) {
		addMuskParam(dt);
	}
	*x = musk_param[dt].first;
	*y = musk_param[dt].second;
}
