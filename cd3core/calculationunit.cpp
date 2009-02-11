#include "calculationunit.h"

CalculationUnit CalculationUnit::flow = CalculationUnit("Flow","cubic meter per seconds","m^3/s");
CalculationUnit CalculationUnit::volume = CalculationUnit("Volume","cubic meter","m^3");
CalculationUnit CalculationUnit::concentration = CalculationUnit("Concentration","gram per cubic meter","g/m^3");

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

bool CalculationUnit::operator == (const ICalculationUnit &other) {
	return (name == other.getName()) && (description == other.getDescritption()) && (unit == other.getUnit());
}
