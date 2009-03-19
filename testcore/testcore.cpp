#include <iostream>
#include <testnode.h>
#include <flow.h>
#include <calculationunit.h>

int main() {
	TestNode t;
	t.init(0, 7200, 300);
	t.f(0, 300);
	t.f(300, 300);

	Flow f;
	f.addUnit("oida", CalculationUnit::rain, 20.0);

	t.setState<Flow>("test", f);

	t.f(600, 300);

	return 0;
}
