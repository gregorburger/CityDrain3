#include "arrayparamtest.h"
#include <boost/lexical_cast.hpp>

CD3_DECLARE_NODE_NAME(ArrayParamTest)

ArrayParamTest::ArrayParamTest() {
	Qe = 0.0;
	addParameter(ADD_PARAMETERS(Qe));
	addParameter(ADD_PARAMETERS(C));
	addOutPort(ADD_PARAMETERS(out));
}

ArrayParamTest::~ArrayParamTest() {
}

bool ArrayParamTest::init(ptime start, ptime stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	out[0] = Qe;
	size_t nconcs = Flow::countUnits(Flow::concentration);
	if (C.size() != nconcs) {
		Logger(Warning) << "Array Parameter C must contain" << nconcs << "elements";
		return false;
	}
	for (size_t i = 0; i < C.size(); i++) {
		out[i+1] = C[i];
	}
	return true;
}


int ArrayParamTest::f(ptime time, int dt) {
	(void) time;
	return dt;
}
