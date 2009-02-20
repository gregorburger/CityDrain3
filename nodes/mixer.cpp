#include "mixer.h"

#include <flow.h>
#include <boost/foreach.hpp>
#include <sstream>
#include <cd3assert.h>

Mixer::Mixer() {
	num_inputs = 2;
	addParameter(ADD_PARAMETERS(num_inputs));
	inputs = 0;
	out = 0;
}

Mixer::~Mixer() {
	if (inputs) {
		for (int i = 0; i < num_inputs; i++) {
			delete inputs[i];
		}
		delete[] inputs;
	}
	if (out)
		delete out;
}

void Mixer::init(int start, int end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	out = new Flow();
	out->prepareData();
	addOutPort(ADD_PARAMETERS_P(out));
	inputs = new Flow*[num_inputs];
	for (int i = 0; i < num_inputs; i++) {
		std::cout << "settin up port " << i << std::endl;
		inputs[i] = new Flow();
		inputs[i]->prepareData();
		std::ostringstream name;
		name << "inputs[" << i << "]";
		addInPort(name.str(), inputs[i]);
	}
}

Flow *mix(Flow **inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;

	Flow *f = new Flow();

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitCount(CalculationUnit::flow) == 1, "unable to mix more than one flow");
		qe += inputs[i]->getUnitData(CalculationUnit::flow)[0];
	}

	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow).at(0);

	f->addUnit(qename, &CalculationUnit::flow, qe);

	for (int c = 0;
		 c < inputs[0]->getUnitCount(CalculationUnit::concentration);
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(0);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f->addUnit(cname, &CalculationUnit::concentration, Ci/qe);
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

