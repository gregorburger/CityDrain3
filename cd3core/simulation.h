#ifndef SIMULATION_H
#define SIMULATION_H

class IModel;
class IController;
class Node;

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

#define CD3_DECLARE_SIMULATION_NAME(simulation) \
const char *simulation::name = #simulation;

#define CD3_DECLARE_SIMULATION(simulation) \
class simulation : public ISimulation { \
public: \
	static const char *name; \
private:

class ISimulation
{
public:
	virtual ~ISimulation(){}
	virtual void addController(IController *controller) = 0;
	virtual void setSimulationParameters(const SimulationParameters &params) {
		sim_param = params;
	}
	virtual SimulationParameters getSimulationParameters() const {
		return sim_param;
	}
	virtual void start(IModel *model) = 0;
protected:
	SimulationParameters sim_param;
};

#endif // SIMULATION_H
