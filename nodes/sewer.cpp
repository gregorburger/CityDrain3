#include "sewer.h"
#include <sstream>
#include <calculationunit.h>
#include <boost/foreach.hpp>

CD3_DECLARE_NODE_NAME(Sewer)

Sewer::Sewer() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(K));
	addParameter(ADD_PARAMETERS(X));
	addParameter(ADD_PARAMETERS(N));
	K = 300;
	X = 0.1;
	N = 11;
}

Sewer::~Sewer() {
}

void Sewer::deinit() {
	for (int i = 0; i < N; i++) {
		delete V[i];
	}
	delete[] V;
}

void Sewer::init(int start, int end, int dt) {
	(void) start;
	(void) end;

	V = new Flow*[N];

	for (int i = 0; i < N; i++) {
		V[i] = new Flow();
		std::ostringstream name;
		name << "V[" << i << "]";
		addState(name.str(), V[i]);
	}

	addMuskParam(dt);
}

typedef CalculationUnit CU;

Flow route(const Flow inflow, Flow *volume, double C_x, double C_y, int dt) {
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.getIth(CalculationUnit::flow, 0);
	double volqe = volume->getIth(CalculationUnit::flow, 0);
	double outqe = inqe * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe) * dt + volqe;

	out.setIth(CU::flow, 0, outqe);

	newvolume.setIth(CU::flow, 0, newvolqe);

	BOOST_FOREACH(std::string cname, inflow.getUnitNames(CU::concentration)) {
		double c0 = 0.5 * outqe + newvolqe/dt;
		double c1 = inqe * inflow.getValue(cname) -	volume->getValue(cname)*(0.5*outqe - volqe/dt);
		newvolume.setValue(cname, std::max(0.0, c1/c0));
		out.setValue(cname, (newvolume.getValue(cname) + volume->getValue(cname)) * 0.5);
	}
	*volume = newvolume;
	return out;
}

int Sewer::f(int time, int dt) {
	(void) time;
	static bool volinited = false;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);

	Flow tmp = in;

	for (int i = 0; i < N; i++) {
		if (!volinited) {
			*V[i] = in;
			V[i]->clear();
		}

		tmp = route(tmp, V[i], C_x, C_y, dt);
	}
	out = tmp;
	volinited = true;

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
