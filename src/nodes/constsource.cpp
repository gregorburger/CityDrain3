#include "constsource.h"

#include <flow.h>

CD3_DECLARE_NODE_NAME(ConstSource)

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow));
	addOutPort(ADD_PARAMETERS(out));
}

ConstSource::~ConstSource() {
}

int ConstSource::f(ptime time, int dt) {
	(void) time;
	return dt;
}

void ConstSource::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = const_flow;
}
