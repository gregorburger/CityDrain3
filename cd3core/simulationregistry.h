#ifndef SIMULATIONREGISTRY_H
#define SIMULATIONREGISTRY_H

#include <map>
#include <string>

class ISimulationFactory;
class ISimulation;

typedef std::map<std::string, const ISimulationFactory *> sim_reg_type;

class SimulationRegistry
{
public:
	SimulationRegistry();
	virtual ~SimulationRegistry();
	void addSimulationFactory(const ISimulationFactory *factory);
	bool contains(const std::string &name) const;
	ISimulation *createSimulation(const std::string &name) const;
private:
	sim_reg_type registry;
};

#endif // SIMULATIONREGISTRY_H
