#include "constsource.h"

#include <flow.h>

CD3_DECLARE_NODE_NAME(ConstSource)

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow))
                .setUnit("m^3/s, g/m^3");
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
	out = const_flow;
	out[0] = out[0] * dt;
	return true;
}
