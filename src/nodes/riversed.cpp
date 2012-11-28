#include "riversed.h"

#include <flow.h>
#include <flowfuns.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "reactor.h"

using namespace boost;


CD3_DECLARE_NODE_NAME(RiverSed)

RiverSed::RiverSed()
{
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));

	/* Muskingum Parameters */
	K = 300;
	X = 0.1;
	N = 11;
	addParameter(ADD_PARAMETERS(K))
			.setUnit("K per subreach in s");
	addParameter(ADD_PARAMETERS(X));
	addParameter(ADD_PARAMETERS(N));

	// Lateral influent from Groundwater in m3/s per river node
	addParameter(ADD_PARAMETERS(LATQ))
			.setUnit("LATQ m^3/s, g/m^3");

	/* Reactor Parameters */
	// SED = No of Substances that are NOT transported with flow
	// !!! MUST BE PLACED AT THE END OF THE FLOW VECTOR
	// SED = 2 means that the last 2 Sunstances in the flow vector are not transported
	SED = 0;
	addParameter(ADD_PARAMETERS(SED))
			.setUnit("-");

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


RiverSed::~RiverSed(){
}


void RiverSed::deinit() {
	for (int i = 0; i < N; i++) {
		Flow *f = getState<Flow>(str(format("V[%1%]") % i));
		removeState(str(format("V[%1%]") % i));
		delete reactors[i];
		delete f;
	}
	reactors.clear();
	V.clear();
}


bool RiverSed::init(ptime start, ptime end, int dt) {
	(void) end;
	for (int i = 0; i < N; i++) {
		V.push_back(new Flow());
		addState(str(format("V[%1%]") % i), V[i]);
		reactors.push_back(new Reactor(*V[i], true));   // Reactor block defined with variable volume (true) - pointer to V[i]
		reactors[i]->nstep = nstep;
		reactors[i]->init_v = init_v;
		if (!reactors[i]->init(dt, constants, conc_formula)) {
			return false;
		}
	}

	addMuskParam(dt);
	return true;
}


int RiverSed::f(ptime time, int dt) {
	(void) time;
	double C_x, C_y;
	setMuskParam(&C_x, &C_y, dt);
	Flow tmp = in;

	// Lateral inflow as Volume per subreach
	double LATQv = LATQ[0]*dt/N;

	for (int i = 0; i < N; i++) {

		// Lateral Inflow - added
		for (int ii = 0; ii < nc; ii++) {
			tmp[ii+1]=(tmp[0]*tmp[ii+1]+LATQv*LATQ[ii+1])/(tmp[0]+LATQv);
		}
		tmp[0] += LATQv;

		// Muskingum routing
		// tmp = Vector inflow
		// *(V[i])= Vector subreach
		// Calculation of hydraulics with timestep dt >> C_x and C_y
		double inqe = tmp[0];
		double volqe = (*(V[i]))[0];
		double outqe = (inqe / dt * C_x + volqe * C_y) * dt;
		double newvolqe = (inqe - outqe) + volqe;
		double dvolqe = (inqe - outqe);

		// Reactor with variable Volume
		reactors[i]->react_dV_SED(tmp, dvolqe,SED, dt);

		// Write Flow and Concentration back in tmp for next subreach inflow
		tmp = (*(V[i]));
		tmp[0] = outqe;

		// Store newvolume in V[i] - just safety/should be OK anyway
		(*(V[i]))[0]=newvolqe;

	}
	out = tmp;
	return dt;
}



void RiverSed::addMuskParam(int dt) {
	double dt_halve = static_cast<double>(dt) / 2;
	double K_1_min_X = K*(1-X);
	double Cx = (dt_halve - K*X) / (dt_halve + K_1_min_X);
	double Cy = 1.0 / (dt_halve+K_1_min_X);
	musk_param[dt] = std::pair<double, double>(Cx, Cy);
}

void RiverSed::setMuskParam(double *x, double *y, int dt) {
	if (musk_param.find(dt) == musk_param.end()) {
		addMuskParam(dt);
	}
	*x = musk_param[dt].first;
	*y = musk_param[dt].second;
}
