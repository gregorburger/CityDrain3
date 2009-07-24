#ifndef ISIMULATIONFACTORY_H
#define ISIMULATIONFACTORY_H

#include <simulation.h>
#include <cd3globals.h>

class CD3_PUBLIC ISimulationFactory
{
public:
	virtual ~ISimulationFactory() {
	}
	virtual ISimulation *createSimulation() const = 0;
	virtual std::string getSimulationName() const = 0;
};

template<class SimTemp>
class CD3_PUBLIC SimulationFactory : public ISimulationFactory {
	virtual ~SimulationFactory() {
	}
	ISimulation *createSimulation() const;
	std::string getSimulationName() const;
};

template<class SimTemp>
ISimulation *SimulationFactory<SimTemp>::createSimulation() const {
	return new SimTemp();
}

template<class SimTemp>
std::string SimulationFactory<SimTemp>::getSimulationName() const {
	return SimTemp::name;
}

#endif // ISIMULATIONFACTORY_H