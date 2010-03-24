#include "cyclenodestart.h"
#include <iostream>
using namespace std;

CD3_DECLARE_NODE_NAME(CycleNodeStart)

CycleNodeStart::CycleNodeStart() {
	addOutPort(ADD_PARAMETERS(out));
	addState("state", &out);
}

CycleNodeStart::~CycleNodeStart() {
}

bool CycleNodeStart::init(ptime start, ptime end, int dt) {
	this->start = start;
	return true;
}

int CycleNodeStart::f(ptime time, int dt) {
	(void) dt;
	if (time == start) {
		out.clear();
	}
	return dt;
}
