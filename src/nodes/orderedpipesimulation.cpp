#include "orderedpipesimulation.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <queue>
#include <set>
#include <QRunnable>
#include <QThreadPool>
#include <QTime>
#include <QProcess>
#include <map>

#include <node.h>
#include <imodel.h>
#include <tqueue.h>
#include <flow.h>
#include "bufferednodeconnection.h"

using namespace std;
using namespace boost;

typedef shared_ptr<tqueue<shared_ptr<Node> > > sh_node_queue;

struct OPSPriv {
	IModel *model;
	vector<shared_ptr<Node> > order;
};

struct OrderedWorker : public QRunnable {
	OrderedWorker(OrderedPipeSimulation *sim, OPSPriv *pd, shared_ptr<Node> last, int time, int dt);
	void run();
	void pullPorts(shared_ptr<Node> sink);
	void pushPorts(shared_ptr<Node> sink);

	sh_node_queue in;
	sh_node_queue out;

	OrderedPipeSimulation *sim;
	OPSPriv *pd;
	shared_ptr<Node> last;
	int time, dt;
};

CD3_DECLARE_SIMULATION_NAME(OrderedPipeSimulation);

OrderedPipeSimulation::OrderedPipeSimulation() {
	pd = new OPSPriv();
}

OrderedPipeSimulation::~OrderedPipeSimulation() {
	delete pd;
}

void OrderedPipeSimulation::start(int time) {
	cd3assert(pd->order.size() > 0, "order empty");
	current_time = time;
	QThreadPool *pool = QThreadPool::globalInstance();
	Q_FOREACH(QString envItem, QProcess::systemEnvironment()) {
		if (envItem.split("=")[0] == "OMP_NUM_THREADS") {
			pool->setMaxThreadCount(envItem.split("=")[1].toInt());
		}
	}
	Logger(Standard) << "thread count: " << pool->maxThreadCount();

	sh_node_queue upper_queue = shared_ptr<tqueue<shared_ptr<Node> > >(new tqueue<shared_ptr<Node> >());
	BOOST_FOREACH(shared_ptr<Node> n, pd->order) {
		upper_queue->enqueue(n);
	}

	shared_ptr<Node> last = pd->order.back();

	QTime start_time = QTime::currentTime();
	while (current_time <= sim_param.stop && running) {
		OrderedWorker *worker = new OrderedWorker(this, pd, last, current_time, sim_param.dt);
		worker->in = upper_queue;
		upper_queue = worker->out;
		pool->start(worker);
		current_time += sim_param.dt;
	}
	pool->waitForDone();
	QTime end_time = QTime::currentTime();
	Logger(Standard) << "simulation took:" << start_time.msecsTo(end_time) << "ms";
}

int OrderedPipeSimulation::run(int time, int dt) {
	(void) time;
	return dt;
}

vector<shared_ptr<Node> > OrderedPipeSimulation::getOrder() {
	queue<shared_ptr<Node> > sources;

	BOOST_FOREACH(shared_ptr<Node> n, pd->model->getSourceNodes()) {
		sources.push(n);
	}
	con_count_type deps = pd->model->getBackwardCounts();
	vector<shared_ptr<Node> > order;
	while (!sources.empty()) {
		shared_ptr<Node> n = sources.front();
		sources.pop();
		order.push_back(n);

		BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(n)) {
			shared_ptr<Node> m = con->sink;
			deps[m] --;
			if (deps[m] == 0) {
				sources.push(m);
			}
		}
	}
	int osize = order.size();
	int all_size = pd->model->getNodes()->size();
	cd3assert(osize == all_size,
			  str(format("order calc wrong size of nodes orderd=%1% all=%2%")
				  % osize % all_size));
	return order;
}

void OrderedPipeSimulation::setModel(IModel *model) {
	cd3assert(model, "model null");
	cd3assert(model->cycleFree(),
			  "The OrderedPipeSimulation does not support cyclic models");
	ISimulation::setModel(model);
	pd->model = model;
	pd->order = getOrder();
}

NodeConnection *OrderedPipeSimulation::createConnection(shared_ptr<Node> source,
														const std::string &srcp,
														shared_ptr<Node> sink,
														const std::string &snkp) const {
	return new BufferedNodeConnection(source, srcp, sink, snkp);
}

OrderedWorker::OrderedWorker(OrderedPipeSimulation *sim,
							 OPSPriv *pd, shared_ptr<Node> last,
							 int _time, int _dt) {
	this->sim = sim;
	this->pd = pd;
	this->last = last;
	this->time = _time;
	this->dt = _dt;
	out = auto_ptr<tqueue<shared_ptr<Node> > >(new tqueue<shared_ptr<Node> >());
}

void OrderedWorker::run() {
	sim->timestep_before(sim, time);
	shared_ptr<Node> current;
	do {
		current = in->dequeue();
		pullPorts(current);
		current->f(time, dt);
		pushPorts(current);
		out->enqueue(current);
	} while (current != last);
	sim->timestep_before(sim, time);
}

void OrderedWorker::pullPorts(shared_ptr<Node> sink) {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(sink)) {
		con->pull(dt);
	}
}

void OrderedWorker::pushPorts(shared_ptr<Node> source) {
	BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(source)) {
		con->push(dt);
	}
}
