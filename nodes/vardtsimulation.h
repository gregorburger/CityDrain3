#ifndef VARDTSIMULATION_H
#define VARDTSIMULATION_H

#include <simulation.h>

class IController;
class SimulationParameters;
class IModel;
class Node;
struct VarDTPriv;

CD3_DECLARE_SIMULATION(VarDTSimulation)
public:
	VarDTSimulation();
	virtual ~VarDTSimulation();
	void addController(IController *c);
	void start(IModel *model);
private:
	int run(Node *n, int time, int dt);

private:
	VarDTPriv *priv;
};

#endif // VARDTSIMULATION_H
