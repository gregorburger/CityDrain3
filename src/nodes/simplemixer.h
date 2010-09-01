#ifndef SIMPLEMIXER_H
#define SIMPLEMIXER_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(SimpleMixer)
public:
	SimpleMixer();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);
private:
	Flow in1, in2;
	Flow out;
};

#endif // SIMPLEMIXER_H
