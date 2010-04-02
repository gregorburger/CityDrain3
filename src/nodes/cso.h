#ifndef CSO_H
#define CSO_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(CSO)
public:
	CSO();
	virtual ~CSO();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);

private:
	Flow in;
	Flow out;
	Flow overflow;
	Flow stored_volume;

	double Q_Max;
	double V_Max;
};

#endif // CSO_H
