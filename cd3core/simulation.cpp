#include "simulation.h"
#include "modelserializer.h"
#include "model.h"
#include "cd3assert.h"
#include "nodeconnection.h"
#include "controller.h"

#include <iostream>
#include <QTime>

ISimulation::ISimulation()
 : running(true) {
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
	cd3assert(model->connected(), "some nodes are not connected");
	m->checkModel();
}

IModel *ISimulation::getModel() const {
	return model;
}

void ISimulation::start(int time) {
	(void) time;
	QTime ts_before = QTime::currentTime();
	current_time = sim_param.start;
	while (running && current_time <= sim_param.stop) {
		timestep_before(this, current_time);
		current_time += run(current_time, sim_param.dt);
		timestep_after(this, current_time);
	}

	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	std::cerr << duration << std::endl;
}

void ISimulation::stop() {
	running = false;
}

void ISimulation::serialize(const std::string &dir) const {
	ModelSerializer ms(model, dir);
	ms.serialize(current_time);
}

void ISimulation::deserialize(const std::string &dir, int time) const {
	ModelSerializer ms(model, dir);
	ms.deserialize(time);
}

NodeConnection *ISimulation::createConnection(Node * source,
								 const std::string &soport,
								 Node *sink,
								 const std::string &siport) const {
	return new NodeConnection(source, soport, sink, siport);
}

