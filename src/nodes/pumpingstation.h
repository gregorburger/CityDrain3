#ifndef PUMPINGSTATION_H
#define PUMPINGSTATION_H

#include <node.h>
#include <flow.h>

#include <vector>

CD3_DECLARE_NODE(PumpingStation)
public:
	PumpingStation();
	virtual ~PumpingStation();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);

private:
	Flow in, out_w, out_p, volume;

	double basin_volume;
	size_t NP;
	std::vector<double> Qp, Von, Voff, Qpp;
	ptime start;
};

#endif // PUMPINGSTATION_H
