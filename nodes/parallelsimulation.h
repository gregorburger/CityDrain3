#ifndef PARALLELSIMULATION_H
#define PARALLELSIMULATION_H

#include <simulation.h>
#include <boost/unordered/unordered_map.hpp>

using namespace boost;

CD3_DECLARE_SIMULATION(ParallelSimulation)
public:
	ParallelSimulation();
	virtual ~ParallelSimulation();

	virtual int run(int time, int dt);

	void setModel(IModel *model);
private:
        unordered_map<Node *, int> createDependsMap() const;
		void run(Node *n, int time, unordered_map<Node *, int> &depends);
		std::vector<Node*> sources;
};

#endif // PARALLELSIMULATION_H
