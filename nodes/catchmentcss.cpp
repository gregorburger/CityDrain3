#include "catchmentcss.h"
#include <flow.h>
#include <flowfuns.h>
#include <calculationunit.h>
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

	addParameter(ADD_PARAMETERS(area));
	addParameter(ADD_PARAMETERS(run_off_coeff));
	addParameter(ADD_PARAMETERS(initial_loss));
	addParameter(ADD_PARAMETERS(permanent_loss));
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

void CatchmentCSS::init(int start, int end, int dt) {
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

typedef CalculationUnit CU;

Flow route(const Flow inflow, Flow rain, Flow *volume, int N, double C_x, double C_y, int dt) {
	//cd3assert(inflow.compatible(rain), "flows not compatible");
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.getIth(CU::flow, 0);
	double volqe = volume->getIth(CU::flow, 0);
	double rainqe = rain.getIth(CU::rain, 0) / N;
	double outqe = (inqe + rainqe) * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe + rainqe) * dt + volqe;

	out.setIth(CU::flow, 0, outqe);

	newvolume.setIth(CU::flow, 0, newvolqe);

	BOOST_FOREACH(std::string cname, inflow.getUnitNames(CU::concentration)) {
		double c0 = 0.5 * outqe + newvolqe/dt;
		double c1 = inqe * inflow.getValue(cname) + rainqe * rain.getValue(cname) - volume->getValue(cname)*(0.5*outqe - volqe/dt);
		newvolume.setValue(cname, std::max(0.0, c1/c0));
		out.setValue(cname, (newvolume.getValue(cname) + volume->getValue(cname)) * 0.5);
	}
	*volume = newvolume;
	return out;
}

int CatchmentCSS::f(int time, int dt) {
	(void) time;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);
	
	std::vector<Flow *> inputs;
	Flow loss = FlowFuns::catchement_lossmodel(*rain_in, loss_basin, initial_loss, permanent_loss, run_off_coeff);
	Flow flow = FlowFuns::catchment_flowmodel(loss, area, dt, rain_con_value, rain_con_name);
	inputs.push_back(&flow);
	if (!dwf_in->empty())
		inputs.push_back(dwf_in);
	if (!parasite_in->empty())
		inputs.push_back(parasite_in);

	Flow rain = inputs.size() > 1 ? FlowFuns::mix(inputs) : flow;

	for (int i = 0; i < N; i++) {
		if (V[i]->empty()) {
			*V[i] = rain;
			V[i]->clear();
		}

		rain = route(*q_upstream, rain, V[i], N, C_x, C_y, dt);
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
