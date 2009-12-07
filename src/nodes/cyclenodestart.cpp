#include "cyclenodestart.h"
#include <iostream>
using namespace std;

CD3_DECLARE_NODE_NAME(CycleNodeStart)

CycleNodeStart::CycleNodeStart() {
	state = &out;
	addOutPort(ADD_PARAMETERS(out));
	addState(ADD_PARAMETERS_P(state));
}

CycleNodeStart::~CycleNodeStart() {

}

int CycleNodeStart::f(int time, int dt) {
	(void) dt;
	(void) time;
	return dt;
}
