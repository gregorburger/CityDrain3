#ifndef REACT_H
#define REACT_H

#include <string>
#include <vector>
#include <flow.h>

namespace mu {
class Parser;
}

class Reactor
{
public:
	Reactor(Flow &volume, bool use_outer_volume = false);
	virtual ~Reactor();
	bool init(int dt,
			  std::string constants,
			  std::vector<std::string> conc_formula);
	void react(Flow &in, int dt);
	void react_dV(Flow &in, double d_reactor_volume, int dt);
	void react_dV_SED(Flow &in, double d_reactor_volume, int SED, int dt);

	std::vector<double> init_v;
	double reactor_volume;
	int nstep;
private:
	Flow &volume;
	double acc;
	std::vector<mu::Parser *> parsers;
	bool use_outer_volume;
};

#endif // REACT_H
