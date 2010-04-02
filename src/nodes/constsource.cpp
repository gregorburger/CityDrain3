#include "constsource.h"

#include <flow.h>

CD3_DECLARE_NODE_NAME(ConstSource)

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow))
		.setUnit("V/s");
	addOutPort(ADD_PARAMETERS(out));
}

ConstSource::~ConstSource() {
}

int ConstSource::f(ptime time, int dt) {
	(void) time;
	return dt;
}

bool ConstSource::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = const_flow;
	const_flow[0] = const_flow[0] * dt;
	return true;
}
