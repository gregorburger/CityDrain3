#ifndef FLOWFUNS_H
#define FLOWFUNS_H

#include <utility>
#include <vector>
#include <string>
class Flow;

void mix(Flow *out, const Flow *const *inputs, int num_inputs);
Flow *mix(const Flow * const *inputs, int num_inputs);
Flow mix(const std::vector<Flow> &inputs);

std::pair<Flow, Flow> split(const Flow f, float ratio);

Flow catchment_flowmodel	(Flow f,
							 int area,
							 int dt,
							 const std::vector<double *> &cvalues,
							 const std::vector<std::string *> &cnames);

Flow catchement_lossmodel	(Flow f,
							 Flow *basin,
							 double init_loss,
							 double perma_loss,
							 double rf_coeff);
Flow muskingum_catchment(Flow f);

#endif // FLOWFUNS_H
