#ifndef RIVER_H
#define RIVER_H

#include <node.h>
#include <flow.h>
#include <vector>
#include <map>

class Reactor;

/* Combine Sewer and Reactor */

CD3_DECLARE_NODE(River)
public:
	River();
	virtual ~River();

	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
	void deinit();

private:
	void addMuskParam(int dt);
	void setMuskParam(double *C_x, double *C_y, int dt);

	/* Stuff from Sewer */
	Flow in;
	Flow out;
	int K;
	double X;
	int N;
	std::vector<Flow*> V;
	std::map<int, std::pair<double, double> > musk_param;

	/* Stuff from ReactorNode */
	std::vector<Reactor*> reactors;
	std::vector<std::string> conc_formula, formula_name;
	std::vector<double> init_v;
	std::string constants;
	int nc, nstep;
};

#endif // SEWER_H
