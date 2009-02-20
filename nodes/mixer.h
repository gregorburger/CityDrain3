#ifndef MIXER_H
#define MIXER_H

#include <node.h>
class Flow;

CD3_DECLARE_NODE(Mixer)
public:
	Mixer();
	~Mixer();
	void f(int time, int dt);
	void init(int start, int end, int dt);
private:
	int num_inputs;
	Flow *out;
	Flow **inputs;
};

#endif // MIXER_H
