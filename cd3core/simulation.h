#ifndef SIMULATION_H
#define SIMULATION_H

class IModel;

struct SimulationParameters {
	SimulationParameters()
		:start(0), stop(7200), dt(300) {

	}
	SimulationParameters(int start,
						 int stop,
						 int dt)

	: start(start), stop(stop), dt(dt) {

	}

	int start;
	int stop;
	int dt;
};

class Simulation
{
public:
	Simulation();
	void setSimulationParameters(const SimulationParameters &params);

	void start(IModel *model);
	void suspend();

private: //methods

private: //data member
	SimulationParameters sp;
};

#endif // SIMULATION_H
