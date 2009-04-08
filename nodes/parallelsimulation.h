#ifndef PARALLELSIMULATION_H
#define PARALLELSIMULATION_H

#include <simulation.h>
#include <tr1/unordered_map>

CD3_DECLARE_SIMULATION(ParallelSimulation)
public:
	ParallelSimulation();
	virtual ~ParallelSimulation();

	virtual int run(int time, int dt);
private:
        std::tr1::unordered_map<Node *, int> createDependsMap() const;
        void run(Node *n, int time, std::tr1::unordered_map<Node *, int> &depends);
	std::vector<Node*> sources;
};

#endif // PARALLELSIMULATION_H
