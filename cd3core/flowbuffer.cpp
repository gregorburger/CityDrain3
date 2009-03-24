#include "flowbuffer.h"

#include <flow.h>
#include <flowfuns.h>
#include <calculationunit.h>
#include <cd3assert.h>

#include <vector>
#include <boost/foreach.hpp>

FlowBuffer::FlowBuffer() {
	buffereddt = 0;
}

FlowBuffer::~FlowBuffer() {
}

void FlowBuffer::put(const Flow f, int dt) {
	assert(dt > 0, "cannot buffer negative dt flow");
	//assert(f, "cannot buffer null flow");
	buffer.push(buf_item(f, dt));
	buffereddt += dt;
}

Flow FlowBuffer::take(int dt) {
	assert(dt <= buffereddt, "cannot take more flow than available");
	//assert(buffer->getUnitNames(CalculationUnit::flow).size() == 1, "no more than onw flow allowed");

	if (buffer.size() == 1) {
		assert(buffereddt == dt, "TODO implement rest mixing in flow buffer");
		Flow single = buffer.front().first;
		buffer.pop();
		buffereddt -= dt;
		return single;
	}

	std::vector<const Flow *> tmp;
	int taken_dt = 0;

	while (taken_dt < dt) {
		buf_item bitem = buffer.front();
		taken_dt += bitem.second;
		tmp.push_back(&bitem.first);
		buffer.pop();
	}
	//std::cout << taken_dt << " - " << dt << std::endl;
	assert(taken_dt == dt, "TODO implement rest mixing in flow buffer");

	Flow mixed = mix(tmp);
	buffereddt -= taken_dt;

	/*BOOST_FOREACH(const Flow *f, tmp) {
		delete f;
	}*/

	//delete all in tmp

	return mixed;
}

int FlowBuffer::buffered() const {
	return buffereddt;
}
