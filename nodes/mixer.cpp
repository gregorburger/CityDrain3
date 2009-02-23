#include "mixer.h"

#include <flow.h>
#include <boost/foreach.hpp>
#include <sstream>
#include <cd3assert.h>
#include <calculationunit.h>

Mixer::Mixer() {
	num_inputs = 2;
	addParameter(ADD_PARAMETERS(num_inputs));
	inited = false;
}

Mixer::~Mixer() {
	if (inited) {
		for (int i = 0; i < num_inputs; i++) {
			delete inputs[i];
		}
		delete[] inputs;
		delete out;
	}

}

void Mixer::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = new Flow();
	addOutPort(ADD_PARAMETERS_P(out));
	inputs = new Flow*[num_inputs];
	for (int i = 0; i < num_inputs; i++) {
		std::cout << "settin up port " << i << std::endl;
		inputs[i] = new Flow();
		std::ostringstream name;
		name << "inputs[" << i << "]";
		addInPort(name.str(), inputs[i]);
	}
	inited = true;
}

Flow *mix(Flow **inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	Flow *f = new Flow();

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}

void Mixer::f(int time, int dt) {
	(void) time;
	(void) dt;
	Flow *mixed = mix(inputs, num_inputs);
	
	*out = *mixed;
	delete mixed;
}

