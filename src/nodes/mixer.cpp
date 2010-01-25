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
typedef pair<string, Flow*> pairtype;
void Mixer::deinit() {
	inputs.clear();

	BOOST_FOREACH(pairtype p, in_ports) {
		removeInPort(p.first);
	}
}

bool Mixer::init(ptime start, ptime end, int dt) {
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
	return true;
}

int Mixer::f(ptime time, int dt) {
	(void) time;
	out = FlowFuns::mix(inputs);
	return dt;
}

