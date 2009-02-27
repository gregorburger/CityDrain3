#ifndef VARDTSIMULATION_H
#define VARDTSIMULATION_H

#include <simulation.h>

class IController;
class SimulationParameters;
class IModel;
class Node;

CD3_DECLARE_SIMULATION(VarDTSimulation)
public:
	VarDTSimulation();
	virtual ~VarDTSimulation();
	void addController(IController *c);
	void start(IModel *model);
private:
	int run(Node *n, int time, int dt);

private:
	IModel *model;
};

#endif // VARDTSIMULATION_H
