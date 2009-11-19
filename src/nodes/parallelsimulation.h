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
	void setNodeDepends() const;
	void run(shared_ptr<Node> n, int time);
	std::vector<shared_ptr<Node> > sources;
};

#endif // PARALLELSIMULATION_H
