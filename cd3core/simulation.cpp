#include "simulation.h"
#include "modelserializer.h"
#include "model.h"
#include "cd3assert.h"
#include "nodeconnection.h"

#include <iostream>
#include <QTime>

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
	cd3assert(model->connected(), "some nodes are not connected");
	m->checkModel();
}

void ISimulation::start(int time) {
	(void) time;
	/*static double one_perc = 1.0f / sim_param.stop;
	static int old_perc_progress = 0;*/

//	current_time = time;
	QTime ts_before = QTime::currentTime();
	while (current_time <= sim_param.stop) {
		//int percent = static_cast<int>(one_perc * current_time * 100);
/*		if (percent != old_perc_progress) {
			old_perc_progress = percent;
			progress(percent);
		}*/
		current_time += run(current_time, sim_param.dt);
		timestep(this, current_time);
	}

	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	std::cerr << duration << std::endl;
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

NodeConnection *ISimulation::createConnection(Node * source,
								 const std::string &soport,
								 Node *sink,
								 const std::string &siport) const {
	return new NodeConnection(source, soport, sink, siport);
}

