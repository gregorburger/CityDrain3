/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include "pipelinedsimulation.h"
#include <imodel.h>
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
	std::map<Node *, ptime>	state;
	QReadWriteLock			state_lock;
	IModel				*model; //StateWorker needs it
};

struct StateWorker : public QRunnable {
	StateWorker(PipeSimPrivate *pd, ptime time, int dt);
	void run();
	bool isRunnable(Node *n) const;
	void updatePorts(Node *n);
	PipeSimPrivate *pd;
	ptime time;
	int dt;
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

void PipelinedSimulation::start(ptime time) {
	running = true;
	QThreadPool *pool = QThreadPool::globalInstance();
	Q_FOREACH(QString envItem, QProcess::systemEnvironment()) {
		if (envItem.split("=")[0] == "OMP_NUM_THREADS") {
			pool->setMaxThreadCount(envItem.split("=")[1].toInt());
		}
	}
	Logger(Standard) << "thread count: " << pool->maxThreadCount();

	BOOST_FOREACH(Node *n, *model->getNodes()) {
		n->start();
	}

	BOOST_FOREACH(Node *n, pd->nodes) {
		pd->state[n] = time;
	}
	QTime ts_before = QTime::currentTime();
	for (current_time = time + seconds(sim_param.dt);
		 current_time <= sim_param.stop;
		 current_time = current_time + seconds(sim_param.dt)) {
		StateWorker *worker = new StateWorker(pd, current_time, sim_param.dt);
		pool->start(worker);
	}
	pool->waitForDone();

	BOOST_FOREACH(Node *n, *model->getNodes()) {
		n->stop();
	}

	QTime ts_after = QTime::currentTime();
	int duration = ts_before.msecsTo(ts_after);
	Logger(Standard) << "simulation took:" << duration << "ms";
}

int PipelinedSimulation::run(ptime time, int dt) {
	cd3assert(false, "don't reach me");
	(void) time;
	return dt;
}


StateWorker::StateWorker(PipeSimPrivate *pd, ptime time, int dt) {
	this->pd = pd;
	this->time = time;
	this->dt = dt;
	this->not_done = pd->nodes;
}

void StateWorker::run() {
	while (!not_done.empty()) {
		node_set_type to_remove;
		BOOST_FOREACH(Node *n, not_done) {
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
				to_remove.insert(n);
				cd3assert(fdt == dt, "PipelinedSimulations dont't support variable dts");
				pd->state[n] += seconds(dt);
			}
			pd->state_lock.unlock();
		}
		BOOST_FOREACH(Node *r, to_remove) {
			not_done.erase(r);
		}
	}
}

void StateWorker::updatePorts(Node *n) {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(n)) {
		con->push(dt);
	}
}

bool StateWorker::isRunnable(Node *n) const {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(n)) {
		Node *dep_node = con->source;
		//assert here for state + dt
		if (pd->state[dep_node] != (time + seconds(dt))) {
			return false;
		}
	}

	BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(n)) {
		Node *dep_node = con->sink;
		//assert here for state + dt
		if (pd->state[dep_node] != time) {
			return false;
		}
	}

	return true;
}
