#ifndef SIMULATION_H
#define SIMULATION_H

#include <boost/signals.hpp>
#include <cd3globals.h>

class IModel;
class Controller;
class Node;
struct NodeConnection;

struct CD3_PUBLIC SimulationParameters {
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
class CD3_PUBLIC simulation : public ISimulation { \
public: \
	static const char *name; \
private:

class CD3_PUBLIC ISimulation {
public:
	ISimulation();
	virtual ~ISimulation();
	virtual void setSimulationParameters(const SimulationParameters &params);
	virtual SimulationParameters getSimulationParameters() const;
	virtual void setModel(IModel *model);
	virtual IModel *getModel() const;
	virtual void start(int time);
	virtual void stop();

	virtual void serialize(const std::string &dir) const;
	virtual void deserialize(const std::string &dir, int time) const;

	virtual int run(int time, int dt) = 0;

	virtual NodeConnection *createConnection(Node * source,
											const std::string &soport,
											Node *sink,
											const std::string &siport) const;

	boost::signal2<void, ISimulation *, int> timestep_after;
	boost::signal2<void, ISimulation *, int> timestep_before;

protected:
	SimulationParameters sim_param;
	IModel *model;
	int current_time;
	bool running;
};

#endif // SIMULATION_H
