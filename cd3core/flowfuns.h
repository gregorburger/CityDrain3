#ifndef FLOWFUNS_H
#define FLOWFUNS_H

#include <utility>
#include <vector>
#include <string>
class Flow;

struct FlowFuns {

//static void mix(Flow *out, const Flow *const *inputs, int num_inputs);
//static Flow *mix(const Flow * const *inputs, int num_inputs);
static Flow mix(const std::vector<Flow*> &inputs);

static std::pair<Flow, Flow> split(const Flow f, float ratio);

static Flow catchment_flowmodel	(Flow f,
							 int area,
							 int dt,
							 const std::vector<double *> &cvalues,
							 const std::vector<std::string *> &cnames);

static Flow catchement_lossmodel	(Flow f,
							 Flow *basin,
							 double init_loss,
							 double perma_loss,
							 double rf_coeff);
//Flow muskingum_catchment(Flow f);
};
#endif // FLOWFUNS_H
