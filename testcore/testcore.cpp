#include <flow.h>
#include <iostream>

int main() {
	Flow f;
	f.addUnit("Q", &CalculationUnit::flow, 4.0);
	f.addUnit("C0", &CalculationUnit::concentration, 1.0);
	f.addUnit("C1", &CalculationUnit::concentration, 299.3);
	f.addUnit("C2", &CalculationUnit::concentration, 3.0);

	std::cout << f.getValue("Q") << std::endl;
	std::cout << f.getValue("C0") << std::endl;
	std::cout << f.getValue("C1") << std::endl;
	std::cout << f.getValue("C2") << std::endl;
	std::cout << f.getValue("C4") << std::endl;
	return 0;
}
