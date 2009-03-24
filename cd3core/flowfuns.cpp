#include "flowfuns.h"
#include <flow.h>
#include <cd3assert.h>
#include <calculationunit.h>

void mix(Flow *f, const Flow * const *inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->setValue(qename, qe);
	//f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		//f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
		f->setValue(cname, Ci/qe);
	}
}

Flow *mix(const Flow * const *inputs, int num_inputs) {
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


Flow mix(const std::vector<const Flow *> &inputs) {
	int num_inputs = inputs.size();
	assert(num_inputs > 1, "cannot mix one input");

	Flow f0 = *inputs[0];
	int flowsize = f0.getUnitNames(CalculationUnit::flow).size();
	assert(flowsize == 1, "unable to mix more than one flow");
	std::string qename = f0.getUnitNames(CalculationUnit::flow)[0];

	Flow f;
	double qe = 0;


	for (int i = 0; i < num_inputs; i++) {
		qe += inputs[i]->getValue(qename);
	}

	f.addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f.addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}

std::pair<Flow, Flow> split(const Flow flow, float ratio) {
	assert(ratio <= 1.0 && ratio >= 0.0, "ratio must me between 0 and 1");
	Flow f1 = flow;
	Flow f2 = flow;

	std::string qename = flow.getUnitNames(CalculationUnit::flow)[0];
	f1.setValue(qename, flow.getValue(qename) * ratio);
	f2.setValue(qename, flow.getValue(qename) * (1 - ratio));

	return std::pair<Flow, Flow>(f1, f2);
}
