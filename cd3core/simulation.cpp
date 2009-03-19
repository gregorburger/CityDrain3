#include "simulation.h"
#include "modelserializer.h"

ISimulation::ISimulation() {
}

ISimulation::~ISimulation() {

}

void ISimulation::setSimulationParameters(const SimulationParameters &params) {
	sim_param = params;
}

SimulationParameters ISimulation::getSimulationParameters() const {
	return sim_param;
}

void ISimulation::setModel(IModel *m) {
	model = m;
}

void ISimulation::start(int time) {
	static double one_perc = 1.0f / sim_param.stop;
	for (current_time = time;
		 current_time <= sim_param.stop;
		 current_time += run(current_time, sim_param.dt)) {

		float percent = one_perc * current_time;
		progress(percent);
		timestep(this, current_time);
	}
}

void ISimulation::addController(IController *c) {
	(void) c;
}

void ISimulation::serialize(const std::string &dir) const {
	ModelSerializer ms(model, dir);
	ms.serialize(current_time);
}

void ISimulation::deserialize(const std::string &dir, int time) const {
	ModelSerializer ms(model, dir);
	ms.deserialize(time);
}
