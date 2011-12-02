#ifndef MCSO_H
#define MCSO_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(mCSO)
public:
	mCSO();
	virtual ~mCSO();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);

private:
	Flow in;
	Flow out;
	Flow overflow;
	Flow stored_volume;

	std::vector<double> efficency;

	double Q_Max;
	double V_Max;
};

#endif // MCSO_H
