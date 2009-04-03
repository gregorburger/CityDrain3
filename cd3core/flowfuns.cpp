#include "flowfuns.h"
#include <flow.h>
#include <cd3assert.h>
#include <calculationunit.h>
#include <set>
#include <boost/foreach.hpp>

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

	cd3assert(in.getUnitNames(CU::rain).size() == 1,
			  "lossmodel: rain flow count must be one");
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

	cd3assert(in.getUnitNames(CU::rain).size() == 1,
			  "flowmodel: rain flow count must be one");
	cd3assert(cvalues.size() == cnames.size(),
			  "flowmodel: concentration values and names must be of same dimensions");
	Flow out;
	double rain = in.getIth(CU::rain, 0);

	for (size_t i = 0; i < cvalues.size(); i++) {
		out.addUnit(*cnames[i], CU::concentration, *cvalues[i]);
	}

	out.addUnit("flow", CU::flow, rain/1000/dt*area);
	return out;
}

Flow FlowFuns::route_sewer(const Flow inflow,
						   Flow *volume,
						   double C_x,
						   double C_y,
						   int dt) {
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.getIth(CalculationUnit::flow, 0);
	double volqe = volume->getIth(CalculationUnit::flow, 0);
	double outqe = inqe * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe) * dt + volqe;

	out.setIth(CU::flow, 0, outqe);

	newvolume.setIth(CU::flow, 0, newvolqe);

	BOOST_FOREACH(std::string cname, inflow.getUnitNames(CU::concentration)) {
		double c0 = 0.5 * outqe + newvolqe/dt;
		double c1 = inqe * inflow.getValue(cname) -	volume->getValue(cname)*(0.5*outqe - volqe/dt);
		newvolume.setValue(cname, std::max(0.0, c1/c0));
		out.setValue(cname, (newvolume.getValue(cname) + volume->getValue(cname)) * 0.5);
	}
	*volume = newvolume;
	return out;
}

Flow FlowFuns::route_catchment(const Flow inflow,
							   Flow rain,
							   Flow *volume,
							   int N,
							   double C_x,
							   double C_y,
							   int dt) {
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.getIth(CU::flow, 0);
	double volqe = volume->getIth(CU::flow, 0);
	double rainqe = rain.getIth(CU::rain, 0) / N;
	double outqe = (inqe + rainqe) * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe + rainqe) * dt + volqe;

	out.setIth(CU::flow, 0, outqe);

	newvolume.setIth(CU::flow, 0, newvolqe);

	BOOST_FOREACH(std::string cname, inflow.getUnitNames(CU::concentration)) {
		double c0 = 0.5 * outqe + newvolqe/dt;
		double c1 = inqe * inflow.getValue(cname) + rainqe * rain.getValue(cname) - volume->getValue(cname)*(0.5*outqe - volqe/dt);
		newvolume.setValue(cname, std::max(0.0, c1/c0));
		out.setValue(cname, (newvolume.getValue(cname) + volume->getValue(cname)) * 0.5);
	}
	*volume = newvolume;
	return out;
}
