#ifndef SIMULATION_H
#define SIMULATION_H

class IModel;
class IController;
class Node;

#include <boost/signals.hpp>

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

class ISimulation {
public:
	ISimulation();
	virtual ~ISimulation();
	virtual void addController(IController *controller);
	virtual void setSimulationParameters(const SimulationParameters &params);
	virtual SimulationParameters getSimulationParameters() const;
	virtual void setModel(IModel *model);
	virtual void start(int time);

	virtual void serialize(const std::string &dir) const;
	virtual void deserialize(const std::string &dir, int time) const;

	virtual int run(int time, int dt) = 0;

	boost::signal1<void, float> progress;
	boost::signal2<void, ISimulation *, int> timestep;

protected:
	SimulationParameters sim_param;
	IModel *model;
	int current_time;
};

#endif // SIMULATION_H
