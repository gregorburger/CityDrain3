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
			.setUnit("s");
	addParameter(ADD_PARAMETERS(X));
	addParameter(ADD_PARAMETERS(N));

	/* Reactor Paramters */
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
		reactors.push_back(new Reactor(*V[i], true));
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
		Flow tmp_in = tmp;
		tmp = FlowFuns::route_sewer(tmp, V[i], C_x, C_y, dt);
		reactors[i]->react(tmp_in, dt);
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
