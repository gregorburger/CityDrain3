#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include <simulation.h>
#include <map>
#include <model.h>

class SimPriv;

CD3_DECLARE_SIMULATION(DefaultSimulation)
public:
    DefaultSimulation();
	~DefaultSimulation();

	int run(int time, int dt);

private: //methods
	void run(Node *n, int time, std::map<Node *, int> &deps);
	std::map<Node *, int> createDependsMap() const;
	void setModel(IModel *model);

private: //data member
	SimPriv *sp;
	node_set_type sources;
};

#endif // DEFAULTSIMULATION_H
