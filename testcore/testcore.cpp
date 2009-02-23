#include <iostream>
#include <flow.h>
#include <calculationunit.h>

int main() {
	Flow f;
	Flow g;

	f.addUnit("oida", CalculationUnit::flow, 3.0);
	return 0;
}
