#include "simulation.h"

#include <model.h>

Simulation::Simulation() {
}

void Simulation::setSimulationParameters(const SimulationParameters &param) {
	sp = param;
}

void Simulation::start(IModel *model) {
	model->initNodes(sp);

	for (int time = sp.start; time < sp.stop; time += sp.dt) {

	}
}
