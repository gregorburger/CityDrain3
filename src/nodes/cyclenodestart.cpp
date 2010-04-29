#include "cyclenodestart.h"
#include <iostream>

CD3_DECLARE_NODE_NAME(CycleNodeStart)

CycleNodeStart::CycleNodeStart() {
	addOutPort(ADD_PARAMETERS(out));
	addState("state", &out);
}

CycleNodeStart::~CycleNodeStart() {
}

bool CycleNodeStart::init(ptime start, ptime end, int dt) {
	out.clear();
	return true;
}

int CycleNodeStart::f(ptime time, int dt) {
	(void) dt;
	(void) time;
	return dt;
}
