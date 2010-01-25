#ifndef MIXER_H
#define MIXER_H

#include <flow.h>

#include <node.h>
#include <vector>

CD3_DECLARE_NODE(Mixer)
public:
	Mixer();
	~Mixer();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);
	void deinit();
private:
	int num_inputs;
	Flow out;
	std::vector<Flow *> inputs;
};

#endif // MIXER_H
