/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
