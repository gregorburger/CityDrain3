#ifndef PARALLELSIMULATION_H
#define PARALLELSIMULATION_H

#include <simulation.h>
#include <map>

CD3_DECLARE_SIMULATION(ParallelSimulation)
public:
	ParallelSimulation();
	virtual ~ParallelSimulation();

	virtual int run(int time, int dt);
private:
	std::map<Node *, int> createDependsMap() const;
	void run(Node *n, int time, std::map<Node *, int> &depends);
	std::vector<Node*> sources;
};

#endif // PARALLELSIMULATION_H
