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
#include <model.h>
#include <tqueue.h>
#include <flow.h>
#include <nodeconnection.h>

using namespace std;
using namespace boost;

struct OPSPriv {
	IModel *model;
	vector<Node *> order;
};

struct OrderedWorker : public QRunnable {
	OrderedWorker(OPSPriv *pd, Node *last, int time, int dt);
	void run();
	void pullPorts(Node *sink);
	void pushPorts(Node *sink);

	shared_ptr<tqueue<Node *> > in;
	shared_ptr<tqueue<Node *> > out;

	OPSPriv *pd;
	Node *last;
	int time, dt;
	std::map<Node *, std::vector<tuples::tuple<Node *, bool> > > siblings;
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
	std::cout << "thread count: " << pool->maxThreadCount() << std::endl;

	shared_ptr<tqueue<Node *> > upper_queue = shared_ptr<tqueue<Node*> >(new tqueue<Node*>());
	BOOST_FOREACH(Node *n, pd->order) {
		upper_queue->enqueue(n);
	}

	Node *last = pd->order.back();

	QTime start_time = QTime::currentTime();
	while (current_time <= sim_param.stop) {
		OrderedWorker *worker = new OrderedWorker(pd, last, current_time, sim_param.dt);
		worker->in = upper_queue;
		upper_queue = worker->out;
		pool->start(worker);
		current_time += sim_param.dt;
	}
	pool->waitForDone();
	QTime end_time = QTime::currentTime();
	std::cerr << start_time.msecsTo(end_time) << std::endl;
}

int OrderedPipeSimulation::run(int time, int dt) {
	(void) time;
	return dt;
}

vector<Node *> OrderedPipeSimulation::getOrder() {
	queue<Node *> sources;

	BOOST_FOREACH(Node *n, pd->model->getSourceNodes()) {
		sources.push(n);
	}
	con_count_type deps = pd->model->getBackwardCounts();
	vector<Node*> order;
	while (!sources.empty()) {
		Node *n = sources.front();
		sources.pop();
		order.push_back(n);

		BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(n)) {
			Node *m = con->sink;
			deps[m] --;
			if (deps[m] == 0) {
				sources.push(m);
			}
		}
	}
	int osize = order.size();
	int all_size = pd->model->getNodes()->size();
	cd3assert(osize == all_size, str(format("order calc wrong size of nodes orderd=%1% all=%2%") % osize % all_size));
	return order;
}

void OrderedPipeSimulation::setModel(IModel *model) {
	cd3assert(model, "model null");
	ISimulation::setModel(model);
	pd->model = model;
	pd->order = getOrder();
}

OrderedWorker::OrderedWorker(OPSPriv *pd, Node *last, int _time, int _dt) {
	this->pd = pd;
	this->last = last;
	this->time = _time;
	this->dt = _dt;
	out = auto_ptr<tqueue<Node *> >(new tqueue<Node *>());
}

void OrderedWorker::run() {
	//std::cout << time << "| starting worker " <<  std::endl;
	Node *current;
	do {
		current = in->dequeue();
		//std::cout << time << " | got node " << pd->model->getNodeName(current) << std::endl;
		pullPorts(current);
		//std::cout << time << " | running node " << pd->model->getNodeName(current) << std::endl;
		current->f(time, dt);
		pushPorts(current);
		/*if (!hasForwardSibling(current))*/
		out->enqueue(current);
	} while (current != last);
	//std::cout << time << "| stopping worker " <<  std::endl;
}

void OrderedWorker::pullPorts(Node *sink) {
	BOOST_FOREACH(NodeConnection *con, pd->model->backwardConnection(sink)) {
		con->pull();
	}
}

void OrderedWorker::pushPorts(Node *source) {
	BOOST_FOREACH(NodeConnection *con, pd->model->forwardConnection(source)) {
		con->push();
	}
}
