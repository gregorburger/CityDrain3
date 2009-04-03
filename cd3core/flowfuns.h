#ifndef FLOWFUNS_H
#define FLOWFUNS_H

#include <utility>
#include <vector>
#include <string>
class Flow;

struct FlowFuns {

static Flow mix(const std::vector<Flow*> &inputs);

static std::pair<Flow, Flow> split(const Flow f,
								   float ratio);

static Flow catchment_flowmodel(Flow f,
								int area,
								int dt,
								const std::vector<double *> &cvalues,
								const std::vector<std::string *> &cnames);

static Flow catchement_lossmodel(Flow f,
								 Flow *basin,
								 double init_loss,
								 double perma_loss,
								 double rf_coeff);

static Flow route_catchment(const Flow inflow,
							Flow rain,
							Flow *volume,
							int N,
							double C_x,
							double C_y,
							int dt);

static Flow route_sewer(const Flow inflow,
						Flow *volume,
						double C_x,
						double C_y,
						int dt);
};
#endif // FLOWFUNS_H
