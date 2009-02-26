#include "flowbuffer.h"
#include <flow.h>
#include <queue>
#include <calculationunit.h>
#include <cd3assert.h>

FlowBuffer::FlowBuffer() {
	buffer = new Flow();
}

FlowBuffer::~FlowBuffer() {
	delete buffer;
}

void FlowBuffer::put(const Flow *f, int dt) {
	assert(dt > 0, "cannot buffer negative dt flow");
	assert(f, "cannot buffer null flow");
	if (!buffereddt) {
		*buffer = *f;
		buffer->copy();
		return;
	}
	const Flow *inputs[2];
	inputs[0] = buffer;
	inputs[1] = f;

	Flow *tmp = mix(inputs, 2);
	delete buffer;
	buffer = tmp;
	buffereddt += dt;
}

Flow *FlowBuffer::take(int dt) {
	assert(dt <= buffereddt, "cannot take more flow than available");
	assert(buffer->getUnitNames(CalculationUnit::flow).size() == 1, "no more than onw flow allowed");
	Flow *f = new Flow(*buffer);
	std::string qename = f->getUnitNames(CalculationUnit::flow)[0];
	double allQe = buffer->getValue(qename);
	double dtQe = static_cast<double>(buffereddt) / dt * allQe;
	buffer->setValue(qename, allQe - dtQe);
	f->setValue(qename, dtQe);
	buffereddt -= dt;
	return f;
}

int FlowBuffer::buffered() const {
	return buffereddt;
}
