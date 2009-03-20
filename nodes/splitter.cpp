#include "splitter.h"
#include <flow.h>

CD3_DECLARE_NODE_NAME(Splitter)

Splitter::Splitter() {
	in = new Flow();
	out1 = new Flow();
	out2 = new Flow();

	addInPort(ADD_PARAMETERS_P(in));
	addOutPort(ADD_PARAMETERS_P(out1));
	addOutPort(ADD_PARAMETERS_P(out2));

	ratio = 0.5;
	addParameter(ADD_PARAMETERS(ratio));
}

int Splitter::f(int time, int dt) {
	(void) time;

	std::pair<Flow, Flow> splitted = split(*in, ratio);
	*out1 = splitted.first;
	*out2 = splitted.second;
	return dt;
}
