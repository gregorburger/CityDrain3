#include "catchmentcss.h"
#include <flow.h>
#include <flowfuns.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace boost;

CD3_DECLARE_NODE_NAME(CatchmentCSS)

CatchmentCSS::CatchmentCSS() {
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

	for (size_t i = 0; i < Flow::countUnits(Flow::concentration); i++) {
		rain_concentration.push_back(0.0);
	}

	addArrayParameter(ADD_PARAMETERS(rain_concentration));

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

	this->start = start;

	size_t nconcs = Flow::countUnits(Flow::concentration);
	while (rain_concentration.size() < nconcs) {
		rain_concentration.push_back(0.0);
	}

	for (int i = 0; i < N; i++) {
		Flow *f = new Flow();
		addState(str(format("V[%1%]") % i), f);
		V.push_back(f);
	}
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
	if (time == start) {
		for (int i = 0; i < N; i++) {
			V[i]->clear();
		}
		loss_basin.clear();
	}

	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	std::vector<Flow> inputs;
	Flow loss = FlowFuns::catchement_lossmodel(rain_in,
											   &loss_basin,
											   initial_loss,
											   permanent_loss / 86400 * dt,
											   run_off_coeff);
	Flow flow = FlowFuns::catchment_flowmodel(loss, area, dt, rain_concentration);

	inputs.push_back(flow);
	inputs.push_back(dwf_in);
	cout << "dwf: "; dwf_in.dump();
	inputs.push_back(parasite_in);
	cout << "parasite_in: "; parasite_in.dump();

	Flow rain = FlowFuns::mix(inputs);
	Flow qi = q_upstream;

	for (int i = 0; i < N; i++) {
		qi = FlowFuns::route_catchment(qi, rain, V[i], N, C_x, C_y, dt);
	}
	out = qi;
	return dt;
}

void CatchmentCSS::initFlows() {
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
