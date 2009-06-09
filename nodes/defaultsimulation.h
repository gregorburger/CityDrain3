#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include <simulation.h>
#include <model.h>

struct SimPriv;

CD3_DECLARE_SIMULATION(DefaultSimulation)
public:
	DefaultSimulation();
	virtual ~DefaultSimulation();

	virtual int run(int time, int dt);

private: //methods
	virtual void run(Node *n, int time, con_count_type &deps);
	virtual void setModel(IModel *model);

private: //data member
	SimPriv *sp;
};

#endif // DEFAULTSIMULATION_H
