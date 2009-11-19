#include "pipelinedsimulation.h"
#include <model.h>
#include <node.h>
#include <nodeconnection.h>

#include <vector>
#include <boost/foreach.hpp>
#include <QReadWriteLock>
#include <QThreadPool>
#include <QProcess>
#include <QTime>

using namespace boost;


CD3_DECLARE_SIMULATION_NAME(PipelinedSimulation)

struct PipeSimPrivate {
	node_set_type		nodes;
	unordered_map<shared_ptr<Node> , int>	state;
	QReadWriteLock			state_lock;
	IModel				*model; //StateWorker needs it
};

struct StateWorker : public QRunnable {
	StateWorker(PipeSimPrivate *pd, int time, int dt);
	void run();
	bool isRunnable(shared_ptr<Node> n) const;
	void updatePorts(shared_ptr<Node> n);
	PipeSimPrivate *pd;
	int time, dt;
	node_set_type not_done;
};

PipelinedSimulation::PipelinedSimulation() {
	pd = new PipeSimPrivate();
}

PipelinedSimulation::~PipelinedSimulation() {
	delete pd;
}

void PipelinedSimulation::setModel(IModel *model) {
	ISimulation::setModel(model);
	pd->model = model;
	pd->nodes = *model->getNodes();
}

void PipelinedSimulation::start(int time) {
	QThreadPool *pool = QThreadPool::globalInstance();
	Q_FOREACH(QString envItem, QProcess::systemEnvironment()) {
		if (envItem.split("=")[0] == "OMP_NUM_THREADS") {
			pool->setMaxThreadCount(envItem.split("=")[1].toInt());
		}
	}
	Logger(Standard) << "thread count: " << pool->maxThreadCount();

	BOOST_FOREACH(shared_ptr<Node> n, pd->nodes) {
		pd->state[n] = time;
	}
	QTime ts_before = QTime::currentTime();
	for (current_time = time;
	     current_time <= sim_param.stop;
	     current_time += sim_param.dt) {
		StateWorker *worker = new StateWorker(pd, current_time, sim_param.dt);
		pool->start(worker);
	}
	pool->waitForDone();
	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	Logger(Standard) << "simulation took:" << duration << "ms";
}

int PipelinedSimulation::run(int time, int dt) {
	cd3assert(false, "don't reach me");
	(void) time;
	return dt;
}


StateWorker::StateWorker(PipeSimPrivate *pd, int time, int dt) {
	this->pd = pd;
	this->time = time;
	this->dt = dt;
	this->not_done = pd->nodes;
}

void StateWorker::run() {
	while (!not_done.empty()) {
		/*node_set_type::iterator it = not_done.begin();
		while (it != not_done.end()) {*/
		node_set_type to_remove;
		BOOST_FOREACH(shared_ptr<Node> n, not_done) {
			//shared_ptr<Node> n = *it;
			pd->state_lock.lockForRead();
			if (pd->state[n] == time) {
				if (!isRunnable(n)) {
					pd->state_lock.unlock();
					continue;
				}
				pd->state_lock.unlock();
				pd->state_lock.lockForWrite();
				updatePorts(n);
				int fdt = n->f(time, dt);
				//not_done.erase(it++);
				to_remove.insert(n);
				cd3assert(fdt == dt, "PipelinedSimulations dont't support variable dts");
				pd->state[n] += dt;
			} /*else {
				it++;
			}*/
			pd->state_lock.unlock();
		}
		BOOST_FOREACH(shared_ptr<Node> r, to_remove) {
			not_done.erase(r);
		}
	}
}

void StateWorker::updatePorts(shared_ptr<Node> n) {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(n)) {
		con->push(dt);
	}
}

bool StateWorker::isRunnable(shared_ptr<Node> n) const {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(n)) {
		shared_ptr<Node> dep_node = con->source;
		//assert here for state + dt
		if (pd->state[dep_node] != (time + dt)) {
			return false;
		}
	}

	BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(n)) {
		shared_ptr<Node> dep_node = con->sink;
		//assert here for state + dt
		if (pd->state[dep_node] != time) {
			return false;
		}
	}

	return true;
}
