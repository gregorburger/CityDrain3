#include "constsource.h"

#include <flow.h>

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow));
	addOutPort(ADD_PARAMETERS(out));
}

ConstSource::~ConstSource() {
}

int ConstSource::f(int time, int dt) {
	(void) time;
	return dt;
}

void ConstSource::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = const_flow;
}
