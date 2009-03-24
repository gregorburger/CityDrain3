#ifndef FLOWFUNS_H
#define FLOWFUNS_H

#include <utility>
#include <vector>
class Flow;

void mix(Flow *out, const Flow *const *inputs, int num_inputs);
Flow *mix(const Flow * const *inputs, int num_inputs);
Flow mix(const std::vector<const Flow *> &inputs);
std::pair<Flow, Flow> split(const Flow f, float ratio);


#endif // FLOWFUNS_H
