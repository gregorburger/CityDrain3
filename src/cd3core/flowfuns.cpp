#include "flowfuns.h"
#include <flow.h>
#include <cd3assert.h>
#include <set>
#include <boost/foreach.hpp>
#include <sstream>

Flow FlowFuns::mix(const std::vector<Flow*> &inputs) {
	std::set<std::string> qe_names;
	Flow f;
	double qe = 0;
	int num_inputs = inputs.size();

	for (int i = 0; i < num_inputs; i++) {
		if (inputs[i]->countUnits(Flow::flow) > 0) {
			qe += inputs[i]->getIth(Flow::flow, 0);
			qe_names.insert(inputs[i]->getUnitNames(Flow::flow)[0]);
			continue;
		}
		if (inputs[i]->countUnits(Flow::rain) > 0) {
			qe += inputs[i]->getIth(Flow::rain, 0);
			qe_names.insert(inputs[i]->getUnitNames(Flow::rain)[0]);
			continue;
		}
		cd3assert(false, "can not mix without either Flow::flow or rain");
	}

	std::stringstream ss;

	ss  << "mixed Flow::flow:";

	BOOST_FOREACH(std::string name, qe_names) {
		ss << " " << name;
	}

	f.addUnit(ss.str(), Flow::flow, qe);
	std::set<std::string> already_mixed;

	for (int i = 0; i < num_inputs; i++) {
		if (!inputs[i]->countUnits(Flow::concentration))
			continue;
		for (size_t c = 0;
			 c < inputs[i]->getUnitNames(Flow::concentration).size();
			 c++) {
			double Ci = 0;
			std::string cname = inputs[i]->getUnitNames(Flow::concentration).at(c);
			if (already_mixed.count(cname) > 0)
				continue;
			for (int ics = 0; ics < num_inputs; ics++) {
				if (!inputs[ics]->hasName(cname))
					continue;
				Ci += inputs[ics]->getValue(cname) * inputs[ics]->getIth(Flow::flow, 0);
			}
			f.addUnit(cname, Flow::concentration, Ci/qe);
			already_mixed.insert(cname);
		}
	}
	return f;
}

std::pair<Flow, Flow> FlowFuns::split(const Flow f, float ratio) {
	cd3assert(ratio <= 1.0 && ratio >= 0.0, "ratio must me between 0 and 1");
	Flow f1 = f;
	Flow f2 = f;

	std::string qename = f.getUnitNames(Flow::flow)[0];
	f1.setValue(qename, f.getValue(qename) * ratio);
	f2.setValue(qename, f.getValue(qename) * (1 - ratio));

	return std::pair<Flow, Flow>(f1, f2);
}

Flow FlowFuns::catchement_lossmodel(Flow in,
						  Flow *basin,
						  double init_loss,
						  double perma_loss,
						  double rf_coeff) {

	cd3assert(in.getUnitNames(Flow::rain).size() == 1,
			  "lossmodel: rain Flow::flow count must be one");
	Flow out = in;
	out.clear();
	if (basin->empty()) {
		*basin = in;
		basin->clear();
	}
	double u = in.getIth(Flow::rain, 0);
	double x = basin->getIth(Flow::rain, 0);

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
	basin->setIth(Flow::rain, 0, x);
	out.setIth(Flow::rain, 0, std::max((u - ( init_loss - x )) * rf_coeff, 0.0));
	return out;
}

Flow FlowFuns::catchment_flowmodel	(Flow in,
							 int area,
							 int dt,
							 const std::vector<double *> &cvalues,
							 const std::vector<std::string *> &cnames) {

	cd3assert(in.getUnitNames(Flow::rain).size() == 1,
			  "Flow::flowmodel: rain Flow::flow count must be one");
	cd3assert(cvalues.size() == cnames.size(),
			  "Flow::flowmodel: concentration values and names must be of same dimensions");
	Flow out;
	double rain = in.getIth(Flow::rain, 0);

	for (size_t i = 0; i < cvalues.size(); i++) {
		out.addUnit(*cnames[i], Flow::concentration, *cvalues[i]);
	}

	out.addUnit("Flow::flow", Flow::flow, rain/1000/dt*area);
	return out;
}

Flow FlowFuns::route_sewer(const Flow inflow,
						   Flow *volume,
						   double C_x,
						   double C_y,
						   int dt) {
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.getIth(Flow::flow, 0);
	double volqe = volume->getIth(Flow::flow, 0);
	double outqe = inqe * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe) * dt + volqe;

	out.setIth(Flow::flow, 0, outqe);

	newvolume.setIth(Flow::flow, 0, newvolqe);

	BOOST_FOREACH(std::string cname, inflow.getUnitNames(Flow::concentration)) {
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
	cd3assert(rain.countUnits(Flow::flow) > 0 || rain.countUnits(Flow::rain) > 0,
			  "rain_in in catchment is empty");
	Flow newvolume = *volume;
	Flow out = inflow;
	double inqe = inflow.countUnits(Flow::flow) ? inflow.getIth(Flow::flow, 0) : 0.0;
	double volqe = volume->getIth(Flow::flow, 0);
	double rainqe = rain.getIth(Flow::flow, 0) / N;
	double outqe = (inqe + rainqe) * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe + rainqe) * dt + volqe;

	out.setIth(Flow::flow, 0, outqe);

	newvolume.setIth(Flow::flow, 0, newvolqe);

	if (inflow.countUnits(Flow::concentration)) {
		BOOST_FOREACH(std::string cname, inflow.getUnitNames(Flow::concentration)) {
			double c0 = 0.5 * outqe + newvolqe/dt;
			double c1 = inqe * inflow.getValue(cname) + rainqe * rain.getValue(cname) - volume->getValue(cname)*(0.5*outqe - volqe/dt);
			newvolume.setValue(cname, std::max(0.0, c1/c0));
			out.setValue(cname, (newvolume.getValue(cname) + volume->getValue(cname)) * 0.5);
		}
	}
	*volume = newvolume;
	return out;
}
