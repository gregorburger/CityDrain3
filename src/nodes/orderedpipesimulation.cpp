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
#include "orderedworker.h"

typedef boost::shared_ptr<tqueue<Node *> > sh_node_queue;

CD3_DECLARE_SIMULATION_NAME(OrderedPipeSimulation);

OrderedPipeSimulation::OrderedPipeSimulation() {
}

OrderedPipeSimulation::~OrderedPipeSimulation() {
}

void OrderedPipeSimulation::start(ptime time) {
	order = getOrder();
	cd3assert(order.size() > 0, "order empty");
	current_time = time;
	QThreadPool *pool = QThreadPool::globalInstance();
	Q_FOREACH(QString envItem, QProcess::systemEnvironment()) {
		if (envItem.split("=")[0] == "OMP_NUM_THREADS") {
			pool->setMaxThreadCount(envItem.split("=")[1].toInt());
		}
	}
	Logger(Standard) << "thread count: " << pool->maxThreadCount();

	sh_node_queue upper_queue = shared_ptr<tqueue<Node *> >(new tqueue<Node *>());
	BOOST_FOREACH(Node *n, order) {
		upper_queue->enqueue(n);
	}

	Node *last = order.back();

	QTime start_time = QTime::currentTime();
	time_iterator titr(sim_param.start, seconds((long)sim_param.dt));
	while (current_time < sim_param.stop && running) {
		OrderedWorker *worker = new OrderedWorker(this, last, current_time, sim_param.dt);
		worker->in = upper_queue;
		upper_queue = worker->out;
		pool->start(worker);
		current_time = current_time + seconds(sim_param.dt);
	}
	pool->waitForDone();
	QTime end_time = QTime::currentTime();
	Logger(Standard) << "simulation took:" << start_time.msecsTo(end_time) << "ms";
}

int OrderedPipeSimulation::run(ptime time, int dt) {
	(void) time;
	return dt;
}

vector<Node *> OrderedPipeSimulation::getOrder() {
		queue<Node *> sources;

		BOOST_FOREACH(Node *n, model->getSourceNodes()) {
		sources.push(n);
	}
	con_count_type deps = model->getBackwardCounts();
	std::vector<Node *> order;
	while (!sources.empty()) {
		Node *n = sources.front();
		sources.pop();
		order.push_back(n);

		BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
			Node *m = con->sink;
			deps[m] --;
			if (deps[m] == 0) {
				sources.push(m);
			}
		}
	}
	int osize = order.size();
	int all_size = model->getNodes()->size();
	cd3assert(osize == all_size,
			  str(format("order calc wrong size of nodes orderd=%1% all=%2%")
				  % osize % all_size));
	return order;
}

NodeConnection *OrderedPipeSimulation::createConnection(Node *source,
														const std::string &srcp,
														Node *sink,
														const std::string &snkp) const {
	return new BufferedNodeConnection(source, srcp, sink, snkp);
}
