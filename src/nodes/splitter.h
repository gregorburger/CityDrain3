#ifndef SPLITTER_H
#define SPLITTER_H

#include <node.h>

class Node;

CD3_DECLARE_NODE(Splitter)
public:
	Splitter();
	virtual ~Splitter();
	int f(int time, int dt);
private:
	Flow *in, *out1, *out2;
	double ratio;
};

#endif // SPLITTER_H
