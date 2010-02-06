#include "simulation.h"
#include "modelserializer.h"
#include "imodel.h"
#include "cd3assert.h"
#include "nodeconnection.h"
#include "log.h"
#include "logger.h"

#include <QTime>
#include <boost/foreach.hpp>

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
	cd3assert(m, "cannot set null model")
	model = m;
}

IModel *ISimulation::getModel() const {
	return model;
}

void ISimulation::start(ptime time) {
	(void) time;
	cd3assert(model, "model not set");
	cd3assert(model->connected(), "model not fully connected");
	if (!model->cycleFree()) {
		Logger(Warning) << "model not cyclefree ";
		BOOST_FOREACH(Node *n, model->cycleNodes()) {
			Logger(Warning) << "node:"  << n << "is a cyclenode";
		}
	}
	cd3assert(model->cycleFree(), "use \"cycle_break\" attribute in connection (xml) to split up cycles");
	model->checkModel();

	QTime ts_before = QTime::currentTime();
	current_time = sim_param.start;
	int dt;
	while (running && current_time <= sim_param.stop) {
		timestep_before(this, current_time);
		dt = run(current_time, sim_param.dt);
		timestep_after(this, current_time);
		current_time = current_time + seconds(dt);
	}

	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	Logger(Standard) << "simulation took:" << duration << "ms";
}

void ISimulation::stop() {
	Logger(Debug) << "interrupted stopping of simulation";
	running = false;
}

void ISimulation::serialize(const std::string &dir) const {
	ModelSerializer ms(model, dir);
	ms.serialize(current_time);
}

void ISimulation::deserialize(const std::string &dir, ptime time) const {
	Logger(Debug) << "serializing timestep" << to_simple_string(time) << "into" << dir;
	ModelSerializer ms(model, dir);
	ms.deserialize(time);
}

NodeConnection *ISimulation::createConnection(Node * source,
								 const std::string &soport,
								 Node *sink,
								 const std::string &siport) const {
	return new NodeConnection(source, soport, sink, siport);
}

