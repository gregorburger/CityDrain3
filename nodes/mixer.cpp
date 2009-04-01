#include "mixer.h"

#include <flow.h>
#include <flowfuns.h>
#include <boost/foreach.hpp>
#include <sstream>
#include <cd3assert.h>
#include <calculationunit.h>

CD3_DECLARE_NODE_NAME(Mixer)

Mixer::Mixer() {
	num_inputs = 2;
	addParameter(ADD_PARAMETERS(num_inputs));
	out = new Flow();
	addOutPort(ADD_PARAMETERS_P(out));
}

Mixer::~Mixer() {
	delete out;
}

void Mixer::deinit() {
	for (size_t i = 0; i < inputs.size(); i++) {
		delete inputs[i];
	}
}

void Mixer::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	for (int i = 0; i < num_inputs; i++) {
		Flow *tmp = new Flow();
		std::ostringstream name;
		name << "inputs[" << i << "]";
		addInPort(name.str(), tmp);
		inputs.push_back(tmp);
	}
}

int Mixer::f(int time, int dt) {
	(void) time;
	*out = FlowFuns::mix(inputs);
	return dt;
}

