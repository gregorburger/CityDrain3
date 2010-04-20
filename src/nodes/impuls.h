#ifndef IMPULS_H
#define IMPULS_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Impuls)
public:
	Impuls();
	virtual ~Impuls();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	Flow out, null, q;
	ptime start, stop;
	double Q;
	int start_after, length;
};

#endif // IMPULS_H
