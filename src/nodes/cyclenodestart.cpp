#include "cyclenodestart.h"

CD3_DECLARE_NODE_NAME(CycleNodeStart)

CycleNodeStart::CycleNodeStart() {
	out = Flow::nullFlow();
	addOutPort(ADD_PARAMETERS(out));
}

CycleNodeStart::~CycleNodeStart() {

}

int CycleNodeStart::f(int time, int dt) {
	(void) dt;
	(void) time;
	return dt;
}
