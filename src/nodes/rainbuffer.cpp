#include "rainbuffer.h"
#include <assert.h>

RainBuffer::RainBuffer() {
}

void RainBuffer::put(int dt, double value) {
	buffer.push_back(rain_event(dt, value));
	avail.push_back(dt);
	this->dt += dt;
}

int RainBuffer::available() const {
	return dt;
}

double RainBuffer::take(int dt) {
	double rain = 0.0;
	while (dt > 0) {
		if (dt >= avail.front()) {
			rain += 1.0 / buffer.front().first * avail.front() * buffer.front().second;
			dt -= avail.front();
			this->dt -= avail.front();
			avail.pop_front();
			buffer.pop_front();
			continue;
		}
		rain += 1.0 / buffer.front().first * dt * buffer.front().second;
		avail.front() -= dt;
		this->dt -= dt;
		dt = 0;
	}
	assert(dt == 0);
	return rain;
}
