#include "cso.h"

#include <flow.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;

#include <cd3assert.h>

CD3_DECLARE_NODE_NAME(CSO)

CSO::CSO() {
	in = new Flow();
	out = new Flow();
	overflow = new Flow();

	addInPort(ADD_PARAMETERS_P(in));
	addOutPort(ADD_PARAMETERS_P(out));
	addOutPort(ADD_PARAMETERS_P(overflow));

	V_Max = 300;
	Q_Max = 0.1;

	addParameter(ADD_PARAMETERS(V_Max));
	addParameter(ADD_PARAMETERS(Q_Max));

	stored_volume = new Flow();
	addState(ADD_PARAMETERS(stored_volume));
}

CSO::~CSO() {
	delete stored_volume;
	delete overflow;
	delete out;
	delete in;
}

void CSO::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
}

int CSO::f(int time, int dt) {
	(void) time;

	if (out->empty()) {
		prepareUnits();
	}

	double Q_In = in->getIth(Flow::flow, 0);
	double V_Stored = stored_volume->getIth(Flow::flow, 0);
	double V_Volume = (Q_In - Q_Max) * dt + V_Stored;
	double Q_Out = 0;
	double Q_Overflow = 0;
	double V_Stored_new = 0;

	if (V_Volume > 0) {
		if (V_Volume < V_Max) {
			//Case 3
			V_Stored_new = (Q_In - Q_Max) * dt - V_Stored;
			Q_Out = Q_Max;
			Q_Overflow = 0;
		} else {
			//Case 2
			V_Stored_new = V_Max;
			Q_Out = Q_Max;
			Q_Overflow = Q_In - Q_Max - (V_Max - V_Stored) / dt;
		}
	} else { //V_Volume < 0
		//Case 1
		V_Stored_new = 0;
		Q_Out = V_Stored  / dt + Q_In;
		Q_Overflow = 0;
	}

	out->setIth(Flow::flow, 0, Q_Out);
	overflow->setIth(Flow::flow, 0, Q_Overflow);
	stored_volume->setIth(Flow::flow, 0, V_Stored_new);

	double V_Prime = Q_In * dt + V_Stored;

	if (V_Prime > 0) {

		cd3assert(V_Prime > 0, str(format("V_Prime (%1% needs to be bigger than 0") % V_Prime));
		double V_Prime_Inv = 1 / V_Prime;

		BOOST_FOREACH(std::string cname, in->getUnitNames(Flow::concentration)) {
			double Ci = (in->getValue(cname) * Q_In * dt
						+ stored_volume->getValue(cname) * V_Stored) * V_Prime_Inv;
			out->setValue(cname, Ci);
			overflow->setValue(cname, Ci);
			stored_volume->setValue(cname, Ci);
		}
	} else {
		Logger(Warning) << "V_prime < 0 ";
	}

	return dt;
}

void CSO::prepareUnits() {
	*out = *in;
	out->clear();
	*stored_volume = *in;
	stored_volume->clear();
	*overflow = *in;
	overflow->clear();
}
