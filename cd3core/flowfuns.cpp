#include "flowfuns.h"
#include <flow.h>
#include <cd3assert.h>
#include <calculationunit.h>
#include <set>

/*void FlowFuns::mix(Flow *f, const Flow * const *inputs, int num_inputs) {
	cd3assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	for (int i = 0; i < num_inputs; i++) {
		cd3assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
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

Flow *FlowFuns::mix(const Flow * const *inputs, int num_inputs) {
	cd3assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	Flow *f = new Flow();

	for (int i = 0; i < num_inputs; i++) {
		cd3assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
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
			Ci += inputs[i]->getValue(cname) ;
		}
		f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}
*/

Flow FlowFuns::mix(const std::vector<Flow*> &inputs) {
	int num_inputs = inputs.size();
	cd3assert(num_inputs > 1, "cannot mix one input");

	Flow f0 = *inputs[0];
	int flowsize = f0.getUnitNames(CalculationUnit::flow).size();
	cd3assert(flowsize == 1, "unable to mix more than one flow");
	std::string qename = f0.getUnitNames(CalculationUnit::flow)[0];

	Flow f;
	double qe = 0;


	for (int i = 0; i < num_inputs; i++) {
		if (inputs[i]->countUnits(CU::flow) > 0) {
			qe += inputs[i]->getIth(CU::flow, 0);
			continue;
		}
		if (inputs[i]->countUnits(CU::rain) > 0) {
			qe += inputs[i]->getIth(CU::rain, 0);
			continue;
		}
		cd3assert(false, "can not mix without either flow or rain");
	}

	f.addUnit(qename, CalculationUnit::flow, qe);

	std::set<std::string> already_mixed;

	for (int i = 0; i < num_inputs; i++) {
		for (size_t c = 0;
			 c < inputs[i]->getUnitNames(CalculationUnit::concentration).size();
			 c++) {
			double Ci = 0;
			std::string cname = inputs[i]->getUnitNames(CalculationUnit::concentration).at(c);
			if (already_mixed.count(cname) > 0)
				continue;
			for (int ics = 0; ics < num_inputs; ics++) {
				if (!inputs[ics]->hasName(cname))
					continue;
				Ci += inputs[ics]->getValue(cname) * inputs[ics]->getIth(CU::flow, 0);
			}
			f.addUnit(cname, CalculationUnit::concentration, Ci/qe);
			already_mixed.insert(cname);
		}
	}
	return f;
}

std::pair<Flow, Flow> FlowFuns::split(const Flow flow, float ratio) {
	cd3assert(ratio <= 1.0 && ratio >= 0.0, "ratio must me between 0 and 1");
	Flow f1 = flow;
	Flow f2 = flow;

	std::string qename = flow.getUnitNames(CalculationUnit::flow)[0];
	f1.setValue(qename, flow.getValue(qename) * ratio);
	f2.setValue(qename, flow.getValue(qename) * (1 - ratio));

	return std::pair<Flow, Flow>(f1, f2);
}

typedef CalculationUnit CU;

Flow FlowFuns::catchement_lossmodel(Flow in,
						  Flow *basin,
						  double init_loss,
						  double perma_loss,
						  double rf_coeff) {

	cd3assert(in.getUnitNames(CU::rain).size() == 1, "lossmodel: rain flow count must be one");
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
	basin->setIth(CU::rain, 0, x);
	out.setIth(CU::rain, 0, std::max( u - ( init_loss - x ) * rf_coeff, 0.0 ) );
	return out;
}

Flow FlowFuns::catchment_flowmodel	(Flow in,
							 int area,
							 int dt,
							 const std::vector<double *> &cvalues,
							 const std::vector<std::string *> &cnames) {
	cd3assert(in.getUnitNames(CU::rain).size() == 1, "flowmodel: rain flow count must be one");
	cd3assert(cvalues.size() == cnames.size(), "flowmodel: concentration values and names must be of same dimensions");
	Flow out;
	double rain = in.getIth(CU::rain, 0);

	for (size_t i = 0; i < cvalues.size(); i++) {
		out.addUnit(*cnames[i], CU::concentration, *cvalues[i]);
	}

	out.addUnit("flow", CU::flow, rain/1000/dt*area);
	return out;
}

