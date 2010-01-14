#include "cyclenodeend.h"
#include "cyclenodestart.h"

CD3_DECLARE_NODE_NAME(CycleNodeEnd)

CycleNodeEnd::CycleNodeEnd() {
	addInPort(ADD_PARAMETERS(in));
	addParameter(ADD_PARAMETERS(start));
}

CycleNodeEnd::~CycleNodeEnd() {
}

int CycleNodeEnd::f(ptime time, int dt) {
	(void) time;
	cd3assert(start, "start not set");
	start->setState("state", in);
	return dt;
}

