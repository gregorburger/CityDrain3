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
#ifndef _MSC_VER
#include <cxxabi.h>
#endif //_MSC_VER

class CD3_PUBLIC ISimulationFactory
{
public:
	virtual ~ISimulationFactory() {
	}
	virtual ISimulation *createSimulation() const = 0;
	virtual std::string getSimulationName() const = 0;
};

template<class T>
class CD3_PUBLIC SimulationFactory : public ISimulationFactory {
	virtual ~SimulationFactory() {
	}
	ISimulation *createSimulation() const;
	std::string getSimulationName() const;
};

template<class T>
ISimulation *SimulationFactory<T>::createSimulation() const {
	return new T;
}

template<class T>
std::string SimulationFactory<T>::getSimulationName() const {
#ifdef _MSC_VER
	std::string name = typeid(T).name();
	return name.substr(name.find(" ") + 1, name.size());
#else
	int status;
	char *cp_class_name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
	std::string class_name(cp_class_name);
	free(cp_class_name);
	return class_name;
#endif
}

#endif // ISIMULATIONFACTORY_H
