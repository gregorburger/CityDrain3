#ifndef PUMPINGSTATION_H
#define PUMPINGSTATION_H

#include <node.h>
#include <flow.h>

#include <vector>

CD3_DECLARE_NODE(PumpingStation)
public:
	PumpingStation();
	virtual ~PumpingStation();
	void init(int start, int end, int dt);
	int f(int time, int dt);

private:
	Flow in, out_w, out_p, volume;

	double basin_volume;
	int NP;
	std::vector<double> Qp, Von, Voff, Qpp;
	int number_of_pollutants;
};

#endif // PUMPINGSTATION_H
