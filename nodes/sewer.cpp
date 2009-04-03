#include "sewer.h"

#include <flow.h>
#include <flowfuns.h>
#include <calculationunit.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;

CD3_DECLARE_NODE_NAME(Sewer)

Sewer::Sewer() {
	in = new Flow();
	out = new Flow();
	addInPort(ADD_PARAMETERS_P(in));
	addOutPort(ADD_PARAMETERS_P(out));
	addParameter(ADD_PARAMETERS(K));
	addParameter(ADD_PARAMETERS(X));
	addParameter(ADD_PARAMETERS(N));
	K = 300;
	X = 0.1;
	N = 11;
}

Sewer::~Sewer() {
	delete in;
	delete out;
}

void Sewer::deinit() {
	for (int i = 0; i < N; i++) {
		delete V[i];
	}
}

void Sewer::init(int start, int end, int dt) {
	(void) start;
	(void) end;

	for (int i = 0; i < N; i++) {
		V.push_back(new Flow());
		addState(str(format("V[%1%]") % i), V[i]);
	}

	addMuskParam(dt);
}


int Sewer::f(int time, int dt) {
	(void) time;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	Flow tmp = *in;

	for (int i = 0; i < N; i++) {
		if (V[i]->empty()) {
			*V[i] = *in;
			V[i]->clear();
		}

		tmp = FlowFuns::route_sewer(tmp, V[i], C_x, C_y, dt);
	}
	*out = tmp;

	return dt;
}

void Sewer::addMuskParam(int dt) {
	double dt_halve = static_cast<double>(dt) / 2;
	double K_1_min_X = K*(1-X);
	double Cx = (dt_halve - K*X) / (dt_halve + K_1_min_X);
	double Cy = 1.0 / (dt_halve+K_1_min_X);
	musk_param[dt] = std::pair<double, double>(Cx, Cy);
}

void Sewer::setMuskParam(double *x, double *y, int dt) {
	if (musk_param.find(dt) == musk_param.end()) {
		addMuskParam(dt);
	}
	*x = musk_param[dt].first;
	*y = musk_param[dt].second;
}
