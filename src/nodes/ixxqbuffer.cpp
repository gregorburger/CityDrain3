#include "ixxqbuffer.h"
#include <assert.h>
#include <boost/foreach.hpp>

IxxQBuffer::IxxQBuffer() {
	clear();
}

void IxxQBuffer::put(int dt, std::vector<double> values) {
    assert(values.size() != 0);
	buffer.push_back(ixx_q_event(dt, values));
	avail.push_back(dt);
	this->dt += dt;
}

int IxxQBuffer::available() const {
	return dt;
}

std::vector<double> IxxQBuffer::take(int dt) {
	assert(dt > 0);
		std::vector<double> flow;

        while (dt > 0) {
                if (dt >= avail.front()) {
						if(flow.size()<buffer.front().second.size())
								flow.resize(buffer.front().second.size(), 0);

                        const double currentrain = 1.0 / buffer.front().first * avail.front() * buffer.front().second[0];
						const double previousrain = flow[0];

                        if(previousrain!=0 || currentrain!=0) {
                                for(unsigned int i=1; i<buffer.front().second.size(); i++)
										flow[i] = (flow[i]*previousrain + buffer.front().second[i]*currentrain)
                                                  / (previousrain+currentrain);
                        }
						flow[0] += currentrain;
                        dt -= avail.front();
                        this->dt -= avail.front();
                        avail.pop_front();
                        buffer.pop_front();
                        continue;
                }

				if(flow.size()<buffer.front().second.size())
						flow.resize(buffer.front().second.size(), 0);

                const double currentrain = 1.0 / buffer.front().first * dt * buffer.front().second[0];
				const double previousrain = flow[0];


                if(previousrain!=0 || currentrain!=0) {
                        for(unsigned int i=1; i<buffer.front().second.size(); i++)
								flow[i] = (flow[i]*previousrain + buffer.front().second[i]*currentrain)
                                          / (previousrain+currentrain);
                }
				flow[0] += currentrain;
                avail.front() -= dt;
                this->dt -= dt;
                dt = 0;
        }
//	assert(dt == 0);
	return flow;
}

void IxxQBuffer::clear() {
	dt = 0;
	avail.clear();
	buffer.clear();
}
