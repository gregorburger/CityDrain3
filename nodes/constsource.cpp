#include "constsource.h"

#include <flow.h>

ConstSource::ConstSource() {
	addParameter(ADD_PARAMETERS(const_flow));
	tmp = new Flow();
}

ConstSource::~ConstSource() {
	delete tmp;
}

void ConstSource::f(int time, int dt) {
	(void) time;
	(void) dt;
}

void ConstSource::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	//out = const_flow;
	addOutPort(ADD_PARAMETERS(out));
}
