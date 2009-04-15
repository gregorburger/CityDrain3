#include "simulation.h"
#include "modelserializer.h"
#include <omp.h>
#include <iostream>

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
	static int old_perc_progress = 0;

	current_time = time;
        double sim_start_time = omp_get_wtime();
	while (current_time <= sim_param.stop) {
		int percent = static_cast<int>(one_perc * current_time * 100);
		if (percent != old_perc_progress) {
			old_perc_progress = percent;
			progress(percent);
		}
		current_time += run(current_time, sim_param.dt);
		timestep(this, current_time);
#pragma omp barrier
	}
        double sim_stop_time = omp_get_wtime();
        double sim_run_time = sim_stop_time - sim_start_time;
        std::cerr << sim_run_time << std::endl;
#pragma omp barrier
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
