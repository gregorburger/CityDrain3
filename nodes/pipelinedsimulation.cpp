#include "pipelinedsimulation.h"
#include <model.h>
#include <node.h>

#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <QReadWriteLock>
#include <QThreadPool>

using namespace boost;


CD3_DECLARE_SIMULATION_NAME(PipelinedSimulation)

struct PipeSimPrivate {
	std::vector<Node *>		nodes;
	unordered_map<Node *, int>	state;
	QReadWriteLock			state_lock;
	IModel				*model; //StateWorker needs it
};

struct StateWorker : public QRunnable {
	StateWorker(PipeSimPrivate *pd, int time, int dt);
	void run();
	bool isRunnable(Node *n) const;
	void updatePorts(Node *n);
	PipeSimPrivate *pd;
	int time, dt;
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
	node_set_type::const_iterator it = model->getNodes()->begin();
	node_set_type::const_iterator end = model->getNodes()->end();
	while (it != end) {
		Node *n = (*it);
		pd->nodes.push_back(n);
		it++;
	}
}

void PipelinedSimulation::start(int time) {
	QThreadPool *pool = QThreadPool::globalInstance();
	std::cout << "thread count: " << pool->maxThreadCount() << std::endl;
	pool->setMaxThreadCount(4);

	BOOST_FOREACH(Node *n, pd->nodes) {
		std::cout << "setting node " << n << " to state " << time << std::endl;
		pd->state[n] = time;
	}

	for (current_time = time;
	     current_time <= sim_param.stop;
	     current_time += sim_param.dt) {
		StateWorker *worker = new StateWorker(pd, current_time, sim_param.dt);
		//worker.setAutoDelete(false);
		pool->start(worker);
	}
	pool->waitForDone();
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
}

void StateWorker::run() {
	//std::cout << "starting StateWorker " << time << std::endl;
	bool finished = true;
	do {
		finished = true;
		//std::cout << "work needs to be done" << std::endl;
		BOOST_FOREACH(Node *n, pd->nodes) {
			pd->state_lock.lockForRead();
			if (pd->state[n] == time) {
				if (!isRunnable(n)) {
					//std::cout << "not runnable" << std::endl;
					pd->state_lock.unlock();
					finished = false;
					continue;
				}
				//std::cout << "runing node " << n << std::endl;
				pd->state_lock.unlock();
				pd->state_lock.lockForWrite();
//QWriteLocker wlocker(&pd->state_lock);
				updatePorts(n);
				int fdt = n->f(time, dt);
				cd3assert(fdt == dt, "PipelinedSimulations dont't support variable dts");
				pd->state[n] += dt;
				finished = false;
			}
			if (pd->state[n] <= time) {
				finished = false;
			}
			pd->state_lock.unlock();
		}
	} while (!finished);

	BOOST_FOREACH (Node *n, pd->nodes) {
		cd3assert(pd->state[n] >= (time + dt), "not really finished");
	}
	std::cout << time << " finished" << std::endl;
}

void StateWorker::updatePorts(Node *n) {
	BOOST_FOREACH(next_node_type next, pd->model->backward(n)) {
		std::string src_port, snk_port;
		Node *src;
		boost::tuples::tie(src_port, src, snk_port) = next;
		n->setInPort(snk_port, src->getOutPort(src_port));
	}
}

bool StateWorker::isRunnable(Node *n) const {
	BOOST_FOREACH(next_node_type next, pd->model->backward(n)) {
		Node *dep_node = next.get<1>();
		//assert here for state + dt
		if (pd->state[dep_node] != (time + dt)) {
			return false;
		}
	}

	BOOST_FOREACH(next_node_type next, pd->model->forward(n)) {
		Node *dep_node = next.get<1>();
		//assert here for state + dt
		if (pd->state[dep_node] != time) {
			return false;
		}
	}

	return true;
}
