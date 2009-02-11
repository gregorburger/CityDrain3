#include <flow.h>
#include <iostream>

int main() {
	Flow *f = new Flow();
	Flow ff;
	*f = ff;
	delete f;
	return 0;
}
