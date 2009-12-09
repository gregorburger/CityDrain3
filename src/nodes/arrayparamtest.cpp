#include "arrayparamtest.h"
#include <boost/lexical_cast.hpp>

CD3_DECLARE_NODE_NAME(ArrayParamTest)

ArrayParamTest::ArrayParamTest() {
	addParameter(ADD_PARAMETERS(Qe));
	addArrayParameter(ADD_PARAMETERS(C));
	addOutPort(ADD_PARAMETERS(out));
}

ArrayParamTest::~ArrayParamTest() {
}

void ArrayParamTest::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	out[0] = Qe;
	for (size_t i = 0; i < C.size(); i++) {
		out[i+1] = C[i];
	}
}


int ArrayParamTest::f(int time, int dt) {
	(void) time;
	return dt;
}
