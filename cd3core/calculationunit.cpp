#include "calculationunit.h"
#include <iostream>

CalculationUnit *CalculationUnit::null = new CalculationUnit("Null","null","null");
CalculationUnit *CalculationUnit::flow = new CalculationUnit("Flow","cubic meter per seconds","m^3/s");
CalculationUnit *CalculationUnit::volume = new CalculationUnit("Volume","cubic meter","m^3");
CalculationUnit *CalculationUnit::concentration = new CalculationUnit("Concentration","gram per cubic meter","g/m^3");
CalculationUnit *CalculationUnit::rain = new CalculationUnit("Rain","rain millimeter per time","mm/s");

CalculationUnit *CalculationUnit::fromString(const std::string &kind) {
	if (kind == "Flow") {
		return CalculationUnit::flow;
	}
	if (kind == "Concentration") {
		return CalculationUnit::concentration;
	}
	if (kind == "Volume") {
		return CalculationUnit::volume;
	}
	if (kind == "Rain") {
		return CalculationUnit::rain;
	}
	return CalculationUnit::null;
}

CalculationUnit::CalculationUnit(const std::string &name, const std::string & description, const std::string &unit)
 : name(name), description(description), unit(unit) {

}

const std::string &CalculationUnit::getName() const {
	return name;
}

const std::string &CalculationUnit::getDescritption() const {
	return description;
}

const std::string &CalculationUnit::getUnit() const {
	return unit;
}

bool CalculationUnit::operator == (const ICalculationUnit &other) const {
	if (this == &other) {
		return true;
	}
	bool b = (name == other.getName());
	std::cout << "CU== " << name << " " << other.getName() << std::endl;
	return b;//&& (description == other.getDescritption()) && (unit == other.getUnit());
}

bool CalculationUnit::operator < (const CalculationUnit &other) const {
	return name < other.name;
}

