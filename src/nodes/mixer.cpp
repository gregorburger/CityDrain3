#include "mixer.h"

#include <flow.h>
#include <flowfuns.h>
#include <boost/foreach.hpp>
#include <sstream>

CD3_DECLARE_NODE_NAME(Mixer)

Mixer::Mixer() {
	num_inputs = 2;
	addParameter(ADD_PARAMETERS(num_inputs));
	addOutPort(ADD_PARAMETERS(out));
}

Mixer::~Mixer() {
}

void Mixer::deinit() {
}

void Mixer::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	for (int i = 0; i < num_inputs; i++) {
		Flow tmp;
		std::ostringstream name;
		name << "inputs[" << i << "]";
		addInPort(name.str(), &tmp);
		inputs.push_back(tmp);
	}
}

int Mixer::f(int time, int dt) {
	(void) time;
	out = FlowFuns::mix(inputs);
	return dt;
}

