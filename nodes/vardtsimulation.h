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
	void setSimulationParameters(const SimulationParameters &sp);
	void start(IModel *model);
private:
	void run(Node *n, int dt);

private:
	IModel *model;
};

#endif // VARDTSIMULATION_H
