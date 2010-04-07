#ifndef WWTP_H
#define WWTP_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(WWTP)
public:
	WWTP();
	virtual ~WWTP();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
	Flow in, out;
	double temperature;
};

#endif // WWTP_H
