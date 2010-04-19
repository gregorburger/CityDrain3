#ifndef SIMPLEPUMP_H
#define SIMPLEPUMP_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(SimplePump)
public:
	SimplePump();

	virtual ~SimplePump();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);
	void deinit();
private:
	Flow in, out_w, out_p, volume;
	double Von, Voff;
	double Qp, Qpp;
	double Vmax;
};

#endif // SIMPLEPUMP_H
