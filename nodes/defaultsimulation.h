#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include <simulation.h>
#include <map>

class SimPriv;

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
	void run(Node *n, int time, std::map<Node *, int> &deps);
	std::map<Node *, int> createDependsMap() const;
	int getMaxDt() const;


private: //data member
	SimPriv *sp;
};

#endif // DEFAULTSIMULATION_H
