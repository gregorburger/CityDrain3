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


Flow mix(const std::vector<Flow> &inputs) {
	int num_inputs = inputs.size();
	assert(num_inputs > 1, "cannot mix one input");

	Flow f0 = inputs[0];
	int flowsize = f0.getUnitNames(CalculationUnit::flow).size();
	assert(flowsize == 1, "unable to mix more than one flow");
	std::string qename = f0.getUnitNames(CalculationUnit::flow)[0];

	Flow f;
	double qe = 0;


	for (int i = 0; i < num_inputs; i++) {
		qe += inputs[i].getValue(qename);
	}

	f.addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0].getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0].getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i].getValue(cname);
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

typedef CalculationUnit CU;

Flow catchement_lossmodel(Flow in,
						  Flow *basin,
						  double init_loss,
						  double perma_loss,
						  double rf_coeff) {

	assert(in.getUnitNames(CU::rain).size() == 1, "lossmodel: rain flow count must be one");
	Flow out = in;
	out.clear();
	if (basin->empty()) {
		*basin = in;
		basin->clear();
	}
	double u = in.getIth(CU::rain, 0);
	double x = basin->getIth(CU::rain, 0);

	if (u > 0) {					// rain
		if (x < init_loss) {
			u = u - init_loss + x;
			if (u < 0)
				x = init_loss + u;
			else
				x = init_loss;
		}
	} else {						// dry weather
		x=std::max(x - perma_loss, 0.0);
	}
	basin->setIth(CU::flow, 0, x);
	out.setIth(CU::flow, 0, std::max( u - ( init_loss - x ) * rf_coeff, 0.0 ) );
	return out;
}

Flow catchment_flowmodel	(Flow in,
							 int area,
							 int dt,
							 const std::vector<double *> &cvalues,
							 const std::vector<std::string> &cnames) {
	assert(in.getUnitNames(CU::rain).size() == 1, "flowmodel: rain flow count must be one");
	assert(cvalues.size() == cnames.size(), "flowmodel: concentration values and names must be of same dimensions");
	Flow out = in;
	double rain = in.getIth(CU::rain, 0);

	for (size_t i = 0; i < cvalues.size(); i++) {
		out.addUnit(cnames[i], CU::concentration, *cvalues[i]);
	}

	out.setIth(CU::flow, 0, rain/1000/dt*area);
	return out;
}

