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
}

Mixer::~Mixer() {
}

void Mixer::deinit() {
	for (int i = 0; i < num_inputs; i++) {
		delete inputs[i];
	}
	delete[] inputs;
	delete out;
}

void Mixer::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = new Flow();
	addOutPort(ADD_PARAMETERS_P(out));
	inputs = new Flow*[num_inputs];
	for (int i = 0; i < num_inputs; i++) {
		inputs[i] = new Flow();
		std::ostringstream name;
		name << "inputs[" << i << "]";
		addInPort(name.str(), inputs[i]);
	}
}

int Mixer::f(int time, int dt) {
	(void) time;
	Flow *mixed = mix(inputs, num_inputs);
	
	*out = *mixed;
	delete mixed;
	return dt;
}

