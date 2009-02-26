#include "vardtsimulation.h"
#include <node.h>
#include <model.h>

VarDTSimulation::VarDTSimulation() {

}

VarDTSimulation::~VarDTSimulation() {

}

void VarDTSimulation::addController(IController *c) {
	(void) c;
}

void VarDTSimulation::setSimulationParameters(const SimulationParameters &sp) {
	(void) sp;
}

void VarDTSimulation::start(IModel *model) {
	this->model = model;
	model->getSinkNodes();
}

void VarDTSimulation::run(Node *n, int dt) {

}
