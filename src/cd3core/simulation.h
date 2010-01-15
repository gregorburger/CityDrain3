#ifndef SIMULATION_H
#define SIMULATION_H

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time.hpp>
using namespace boost;
using namespace boost::posix_time;

#include <cd3globals.h>

class IModel;
class Controller;
class Node;
struct NodeConnection;

struct CD3_PUBLIC SimulationParameters {
	SimulationParameters(){}
	SimulationParameters(std::string start,
						 std::string stop,
						 std::string dt) {
		this->start = time_from_string(start);
		this->stop = time_from_string(stop);
		this->dt = lexical_cast<int>(dt);
	}
	SimulationParameters(ptime start,
						 ptime stop,
						 int dt) : start(start), stop(stop), dt(dt) {

	}

	ptime start;
	ptime stop;
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
	virtual void start(ptime time);
	virtual void stop();

	virtual void serialize(const std::string &dir) const;
	virtual void deserialize(const std::string &dir, ptime time) const;

	virtual int run(ptime time, int dt) = 0;

	virtual NodeConnection *createConnection(Node *source,
											 const std::string &soport,
											 Node *sink,
											 const std::string &siport) const;

	boost::signal2<void, ISimulation *, ptime> timestep_after;
	boost::signal2<void, ISimulation *, ptime> timestep_before;

protected:
	SimulationParameters sim_param;
	IModel *model;
	ptime current_time;
	bool running;
};

#endif // SIMULATION_H
