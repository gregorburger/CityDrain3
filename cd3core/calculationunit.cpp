#include "calculationunit.h"
#include <iostream>

CalculationUnit *CalculationUnit::null = new CalculationUnit("Null","null","null");
CalculationUnit *CalculationUnit::flow = new CalculationUnit("Flow","cubic meter per seconds","m^3/s");
CalculationUnit *CalculationUnit::volume = new CalculationUnit("Volume","cubic meter","m^3");
CalculationUnit *CalculationUnit::concentration = new CalculationUnit("Concentration","gram per cubic meter","g/m^3");


/*CalculationUnit CalculationUnit::null() {
	return CalculationUnit("Null","null","null");
}
CalculationUnit CalculationUnit::flow() {
	return CalculationUnit("Flow","cubic meter per seconds","m^3/s");
}
CalculationUnit CalculationUnit::volume() {
	return CalculationUnit("Volume","cubic meter","m^3");
}

CalculationUnit CalculationUnit::concentration() {
	return CalculationUnit("Concentration","gram per cubic meter","g/m^3");
}
*/
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

/*bool operator == (const CalculationUnit &lhs, const CalculationUnit &rhs) {
	if (&lhs == &rhs) {
		return true;
	}

	std::cout << "CU== " << lhs.getName() << " " << rhs.getName() << std::endl;

	return lhs.getName() == rhs.getName();
}
*/

bool CalculationUnit::operator < (const CalculationUnit &other) const {
	return name < other.name;
}

