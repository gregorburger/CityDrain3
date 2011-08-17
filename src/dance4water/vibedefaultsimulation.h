#ifndef VIBEDEFAULTSIMULATION_H
#define VIBEDEFAULTSIMULATION_H

#include <simulation.h>
#include <imodel.h>

struct SimPriv;

CD3_DECLARE_SIMULATION(VIBeDefaultSimulation)
public:
        VIBeDefaultSimulation();
        virtual ~VIBeDefaultSimulation();

	virtual void start(ptime time);
	int run(ptime time, int dt);
	void setModel(IModel *model);

private:
	void run(Node *n, ptime time, con_count_type &deps);

private: //data member
	SimPriv *sp;
};

#endif // VIBEDEFAULTSIMULATION_H
