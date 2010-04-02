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
	addArrayParameter(ADD_PARAMETERS(Qp));
	addArrayParameter(ADD_PARAMETERS(Von));
	addArrayParameter(ADD_PARAMETERS(Voff));

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

	push_back(Qpp).repeat(NP, 0.0);

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

double sum(const vector<double> &in) {
	return accumulate(in.begin(), in.begin(), 0);
}

int PumpingStation::f(ptime time, int dt) {
	(void) time;

	double qin = in.getIth(Flow::flow, 0);
	double vi = volume.getIth(Flow::flow, 0);
	vector<double> Qpmod, Qpplast;
	Qpplast = Qpp;

	Qpmod += 0.0;
	Qpmod.insert(Qpmod.end(), Qp.begin(), Qp.end());
	vector<double> vik(NP);
	for (size_t i = 0; i < NP; i++) {
		vik[i] = vi + qin * dt - accumulate(Qpmod.begin(), Qpmod.begin()+i, 0) * dt;
		//Qopt1=(((Vik(k)-Voff(k))/tstep)+abs((Vik(k)-Voff(k))/tstep))/2;
		double Qopt1_tmp = (vik[i]-Voff[i])/dt;
		double Qopt1 = (( Qopt1_tmp ) + abs( Qopt1_tmp )) / 2;
		//(Vik(k)>Von(k)) | (Qpplast(k)==Qp(k))
		Qpp[i] = 0.0;
		if (vik[i] > Von[i] || Qpplast[i] == Qp[i]) {
			Qpp[i] = min(Qopt1, Qp[i]);
		}

		//Vii=Vi+qi.*tstep-Vpp; % Volume after pumping
		double Vii = qin*dt-sum(Qpp)*dt;
		out_w.clear();
		if (Vii > basin_volume) {
			out_w.setIth(Flow::flow, 0, (Vii-basin_volume) * dt);
			Vii = basin_volume;
		}
		//Vip=qi*tstep+Vi; % Virtual volume after inflow
		double Vip = qin * dt + vi;
		BOOST_FOREACH(string c, in.getUnitNames(Flow::concentration)) {
			double Cin = in.getValue(c);
			double C = volume.getValue(c);
			//Cprime=(Cin.*qi.*tstep+C.*Vi)./Vip;
			double cprime = (Cin*qin*dt+C*vi) / Vip;
			out_w.setValue(c, cprime);
			out_w.setValue(c, cprime);
			volume.setValue(c, cprime);
		}
		out_p.setIth(Flow::flow, 0, sum(Qpp));
		volume.setIth(Flow::flow, 0, Vii);
	}

	return dt;
}
