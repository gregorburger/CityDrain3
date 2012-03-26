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

#include "pumpingstation.h"
#include <flowfuns.h>
#include <sstream>
#include <numeric>
#include <boost/assign.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::assign;

CD3_DECLARE_NODE_NAME(PumpingStation)

PumpingStation::PumpingStation() {
	basin_volume = 360;

	addParameter(ADD_PARAMETERS(basin_volume))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Qp))
		.setUnit("m^3/s");
	addParameter(ADD_PARAMETERS(Von))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Voff))
		.setUnit("m^3");

	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out_p));
	addOutPort(ADD_PARAMETERS(out_w));
	addState(ADD_PARAMETERS(volume));
}

bool PumpingStation::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;

	if (Qp.size() != Von.size() || Von.size() != Voff.size()) {
		Logger(Warning) << "Array parameters Qp, Von and Voff must be of same dimension";
		return false;
	}

	if (Qp.size() < 1) {
		Logger(Warning) << "At least one Pump must me be given";
		return false;
	}

	NP = Von.size();

	for (size_t i = 0; i < NP; ++i) {
		if (Von[i] < 0 || Von[i] > basin_volume) {
			Logger(Warning) << "Von[i] ";
			return false;
		}

		if (Voff[i] < 0 || Voff[i] > basin_volume) {
			Logger(Warning) << "Von[i] ";
			return false;
		}
		if (Von[i] < Voff[i]) {
			Logger(Warning) << "Von[i] must be smaller than Voff[i]";
			return false;
		}
	}

	//push_back(Qpp).repeat(NP, 0.0);
	Qpp = std::vector<double>(NP);
	Qpplast = std::vector<double>(NP);

	for (size_t i = 0; i < NP - 1; ++i) {
		if (Von[i] > Von[i+1]) {
			Logger(Warning) << "Von[i] must be smaller than Von[i+1]";
			return false;
		}
		if (Voff[i] > Voff[i+1]) {
			Logger(Warning) << "Voff[i] must be smaller than Voff[i+1]";
			return false;
		}
	}
	out_p = in;
	out_p.clear();
	out_w = in;
	out_w.clear();
	volume = in;
	volume.clear();
	return true;
}

PumpingStation::~PumpingStation() {
}

double sum(const std::vector<double> &in) {
	return accumulate(in.begin(), in.begin(), 0);
}

int PumpingStation::f(ptime time, int dt) {
	(void) time;

	double qi = in[0];
	double Vi = volume[0];
	Qpplast = Qpp;

	std::vector<double> vik(NP);
	for (size_t i = 0; i < NP; i++) {
		double qpsum = 0.0;
		for (size_t k = 0; k < i; k++) {
			qpsum += Qp[k];
		}
		vik[i] = Vi + qi * dt - qpsum * dt;

		double Qopt1 = std::max(0.0, (vik[i]-Voff[i])/dt);

		//(Vik(k)>Von(k)) | (Qpplast(k)==Qp(k))
		Qpp[i] = 0.0;
		if ((vik[i] > Von[i]) || (Qpplast[i] == Qp[i])) {
			Qpp[i] = min(Qopt1, Qp[i]);
		}
	}

	//Vii=Vi+qi.*tstep-Vpp; % Volume after pumping
	double Vpp = 0.0;
	for (size_t k = 0; k < Qpp.size(); k++) {
		Vpp += Qpp[k];
	}
	Vpp *= dt;
	double Vii = Vi+qi*dt-Vpp;
	out_w.clear();
	if (Vii > basin_volume) {
		out_w[0] = (Vii-basin_volume) / dt;
		Vii = basin_volume;
	}
	//Vip=qi*tstep+Vi; % Virtual volume after inflow
	double Vip = qi * dt + Vi;
	for (size_t c = 1; c < Flow::size(); c++) {
		double Cin = in[c];
		double C = volume[c];
		//Cprime=(Cin.*qi.*tstep+C.*Vi)./Vip;
		double cprime = (Cin*qi*dt+C*Vi) / Vip;
		out_w[c] = cprime;
		out_p[c] = cprime;
		volume[c] = cprime;
	}

	out_p[0] = 0;
	for (size_t k = 0; k < Qpp.size(); k++) {
		out_p[0] += Qpp[k];
	}

	volume[0] = Vii;

	return dt;
}
