#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include <simulation.h>
#include <imodel.h>

struct SimPriv;

CD3_DECLARE_SIMULATION(DefaultSimulation)
public:
	DefaultSimulation();
	virtual ~DefaultSimulation();

	int run(ptime time, int dt);

	void run(Node *n, ptime time, con_count_type &deps);
	void setModel(IModel *model);

private: //data member
	SimPriv *sp;
};

#endif // DEFAULTSIMULATION_H
