#include "cyclenodeend.h"
#include "cyclenodestart.h"

CD3_DECLARE_NODE_NAME(CycleNodeEnd)

CycleNodeEnd::CycleNodeEnd() {
	addInPort(ADD_PARAMETERS(in));
}

CycleNodeEnd::~CycleNodeEnd() {
}

int CycleNodeEnd::f(int time, int dt) {
	(void) time;
	cd3assert(start, "start not set");
	start->out = in;
	return dt;
}

