#include <iostream>
#include <flow.h>
#include <calculationunit.h>

int main() {
	Flow f;

	f.addUnit("oida", CalculationUnit::flow, 100.0);
	f.addUnit("sick", CalculationUnit::concentration, 4.0);

	std::pair<Flow, Flow> splitted = split(f, 0.2);

	std::cout << splitted.first.getValue("oida") << std::endl;
	std::cout << splitted.first.getValue("sick") << std::endl;
	std::cout << std::endl;

	std::cout << splitted.second.getValue("oida") << std::endl;
	std::cout << splitted.second.getValue("sick") << std::endl;


	return 0;
}
