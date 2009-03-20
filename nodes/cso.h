#ifndef CSO_H
#define CSO_H

#include <node.h>
#include <flow.h>
//class Flow;

CD3_DECLARE_NODE(CSO)
public:
	CSO();
	virtual ~CSO();
	int f(int time, int dt);
	void init(int start, int end, int dt);
private:
	void prepareUnits();

	Flow *in;
	Flow *out;
	Flow *overflow;
	Flow *stored_volume;

	double Q_Max;
	double V_Max;
};

#endif // CSO_H
