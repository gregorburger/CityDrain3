#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include "simulation.h"

#include <vector>
#include <map>

CD3_DECLARE_SIMULATION(DefaultSimulation)
public:
    DefaultSimulation();
	~DefaultSimulation();

	void start(IModel *model);
	void setSimulationParameters(const SimulationParameters &param);
	SimulationParameters getSimulationParameters() const;
	void suspend();
	void addController(IController *controller);

private: //methods
	void run(IModel *model, Node *n, int time);
	std::map<Node *, int> getDepends(IModel *model);
private: //data member
	SimulationParameters sp;
	std::vector<IController *> controller;
};

#endif // DEFAULTSIMULATION_H
