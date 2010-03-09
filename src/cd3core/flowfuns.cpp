#include "flowfuns.h"
#include <flow.h>
#include <cd3assert.h>
#include <set>
#include <boost/foreach.hpp>
#include <sstream>

Flow FlowFuns::mix(const std::vector<Flow*> &inputs) {
	Flow f;
	for (size_t i = 0; i < inputs.size(); i++) {
		f[0] += (*inputs[i])[0];
	}

	for (size_t cn = 0; cn < f.countUnits(Flow::concentration); cn++) {
		double c = 0.0;
		for (size_t i = 0; i <  inputs.size(); i++) {
			c += (*inputs[i])[cn+1] * (*inputs[i])[0];
		}
		f[cn+1] = c/f[0];
	}
	return f;
}

Flow FlowFuns::mix(const std::vector<Flow> &inputs) {
	Flow f;
	for (size_t i = 0; i < inputs.size(); i++) {
		f[0] += inputs[i][0];
	}

	for (size_t cn = 0; cn < f.countUnits(Flow::concentration); cn++) {
		double c = 0.0;
		for (size_t i = 0; i <  inputs.size(); i++) {
			c += inputs[i][cn+1] * inputs[i][0];
		}
		f[cn+1] = c/f[0];
	}
	return f;
}

std::pair<Flow, Flow> FlowFuns::split(const Flow f, float ratio) {
	cd3assert(ratio <= 1.0 && ratio >= 0.0, "ratio must me between 0 and 1");
	Flow f1;
	Flow f2;

	double qe = f[0];
	f1[0] = qe * ratio;
	f2[0] = qe * (1 - ratio);

	return std::pair<Flow, Flow>(f1, f2);
}

Flow FlowFuns::catchement_lossmodel(Flow in,
						  Flow *basin,
						  double init_loss,
						  double perma_loss,
						  double rf_coeff) {

	Flow out;
	double u = in[0];
	double x = (*basin)[0];

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
	(*basin)[0] = x;
	out[0] = std::max((u - ( init_loss - x )) * rf_coeff, 0.0);
	return out;
}

Flow FlowFuns::catchment_flowmodel	(Flow in,
							 int area,
							 int dt,
							 const std::vector<double> &cvalues) {

	cd3assert(cvalues.size() == Flow::countUnits(Flow::concentration),
			  "wrong size of concentration values vector");
	Flow out;
	double rain = in[0];

	for (size_t i = 0; i < cvalues.size(); i++) {
		out[i+1] = cvalues[i];
	}

	out[0] = rain/1000/dt*area;
	return out;
}

Flow FlowFuns::route_sewer(const Flow inflow,
						   Flow *volume,
						   double C_x,
						   double C_y,
						   int dt) {
	double inqe = inflow[0];
	double volqe = (*volume)[0];
	double outqe = inqe * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe) * dt + volqe;

	Flow out;
	out[0] = outqe;

	Flow newvolume;
	newvolume[0] = newvolqe;

	for (size_t i = 0; i < Flow::countUnits(Flow::concentration); i++) {
		double c0 = 0.5 * outqe + newvolqe/dt;
		double c1 = inqe * inflow[i+1] - (*volume)[i+1]*(0.5*outqe - volqe/dt);
		newvolume[i+1] = std::max(0.0, c1/c0);
		out[i+1] = (newvolume[i+1] + (*volume)[i+1]) * 0.5;
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
	Flow newvolume;
	Flow out;
	double inqe = inflow.countUnits(Flow::flow) ? inflow[0] : 0.0;
	double volqe = (*volume)[0];
	double rainqe = rain[0] / N;
	double outqe = (inqe + rainqe) * C_x + volqe * C_y;
	double newvolqe = (inqe - outqe + rainqe) * dt + volqe;

	out[0] = outqe;

	newvolume[0] = newvolqe;

	if (inflow.countUnits(Flow::concentration)) {
		for (size_t i = 0; i < Flow::countUnits(Flow::concentration); i++) {
			double c0 = 0.5 * outqe + newvolqe/dt;
			double c1 = inqe * inflow[i+1] + rainqe * rain[i+1] - (*volume)[i+1]*(0.5*outqe - volqe/dt);
			newvolume[i+1] = std::max(0.0, c1/c0);
			out[i+1] = (newvolume[i+1] + (*volume)[i+1]) * 0.5;
		}
	}
	*volume = newvolume;
	return out;
}
