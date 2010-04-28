#ifndef SIMULATIONREGISTRY_H
#define SIMULATIONREGISTRY_H

#include <map>
#include <string>
#include <vector>
#include <cd3globals.h>

class ISimulationFactory;
class ISimulation;

typedef std::map<std::string, const ISimulationFactory *> sim_reg_type;

class CD3_PUBLIC SimulationRegistry
{
public:
	SimulationRegistry();
	virtual ~SimulationRegistry();
	void addSimulationFactory(const ISimulationFactory *factory);
	void addNativePlugin(const std::string &plugin_path);
	std::vector<std::string> getRegisteredNames() const;
	bool contains(const std::string &name) const;
	ISimulation *createSimulation(const std::string &name) const;
private:
	sim_reg_type registry;
};

typedef void (*regSimFunProto)(SimulationRegistry *reg);

#endif // SIMULATIONREGISTRY_H
