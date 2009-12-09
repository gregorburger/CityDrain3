#include "null.h"

CD3_DECLARE_NODE_NAME(Null)

Null::Null() {
	addOutPort("out", &out);
}

int Null::f(int time, int dt) {
	(void) time;
	return dt;
}
