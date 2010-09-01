#include "simplemixer.h"

CD3_DECLARE_NODE_NAME(SimpleMixer)

SimpleMixer::SimpleMixer() {
	addInPort(ADD_PARAMETERS(in1));
	addInPort(ADD_PARAMETERS(in2));
	addOutPort(ADD_PARAMETERS(out));
}

bool SimpleMixer::init(ptime start, ptime end, int dt) {
	return true;
}

int SimpleMixer::f(ptime time, int dt) {
	out[0] = in1[0] + in2[0];
	for (int i = 1; i < Flow::size(); i++) {
		out[i] = (in1[1]*in1[0] + in2[i]*in2[0]) / out[0];
	}
	return dt;
}
