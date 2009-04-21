#include "null.h"

CD3_DECLARE_NODE_NAME(Null)

Null::Null() {
	addInPort("in", &in_out);
	addOutPort("out", &in_out);
}

int Null::f(int time, int dt) {
	(void) time;
	return dt;
}
