#include "catchmentcss.h"
#include <flow.h>
#include <flowfuns.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace boost;

CD3_DECLARE_NODE_NAME(CatchmentCSS)

CatchmentCSS::CatchmentCSS() {
	rain_in = new Flow();
	dwf_in = new Flow();
	parasite_in = new Flow();
	q_upstream = new Flow();
	out = new Flow();
	loss_basin = new Flow();

	addInPort(ADD_PARAMETERS_P(rain_in));
	addInPort(ADD_PARAMETERS_P(dwf_in));
	addInPort(ADD_PARAMETERS_P(parasite_in));
	addInPort(ADD_PARAMETERS_P(q_upstream));

	addOutPort(ADD_PARAMETERS_P(out));

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
	addParameter(ADD_PARAMETERS(n_rain_conc));

	addState(ADD_PARAMETERS_P(loss_basin));

	area = 208;
	run_off_coeff = 0.2;
	initial_loss = 1.0;
	permanent_loss = 1.25;
	N = 3;
	K = 300;
	X = 0;
	n_rain_conc = 0;
}


CatchmentCSS::~CatchmentCSS() {
	delete out;
	delete q_upstream;
	delete parasite_in;
	delete dwf_in;
	delete rain_in;
	delete loss_basin;
}

bool CatchmentCSS::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	(void) dt;

	for (int i = 0; i < n_rain_conc; i++) {
		double *rc = new double;
		std::string *rn = new std::string();
		rain_con_value.push_back(rc);
		rain_con_name.push_back(rn);
		addParameter(str(format("rain_con_value[%1%]") % i), rc);
		addParameter(str(format("rain_con_name[%1%]") % i), rn);
	}

	for (int i = 0; i < N; i++) {
		Flow *f = new Flow();
		addState(str(format("V[%1%]") % i), f);
		V.push_back(f);
	}
	permanent_loss = permanent_loss / 86400 * dt;
	return true;
}

void CatchmentCSS::deinit() {

	for (int i = 0; i < n_rain_conc; i++) {
		delete rain_con_name[i];
		delete rain_con_value[i];
	}

	for (int i = 0; i < N; i++) {
		delete V[i];
	}
}

int CatchmentCSS::f(ptime time, int dt) {
	(void) time;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	std::vector<Flow> inputs;
	Flow loss = FlowFuns::catchement_lossmodel(*rain_in, loss_basin, initial_loss, permanent_loss, run_off_coeff);
	Flow flow = FlowFuns::catchment_flowmodel(loss, area, dt, rain_con_value, rain_con_name);
	inputs.push_back(flow);
	inputs.push_back(*dwf_in);
	inputs.push_back(*parasite_in);

	Flow rain = FlowFuns::mix(inputs);

	for (int i = 0; i < N; i++) {
		rain = FlowFuns::route_catchment(*q_upstream, rain, V[i], N, C_x, C_y, dt);
	}
	*out = rain;
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
