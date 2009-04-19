#include "orderedpipesimulation.h"
#include <boost/foreach.hpp>
#include <queue>
#include <QRunnable>
#include <QThreadPool>
#include <QTime>
#include <QProcess>

#include <node.h>
#include <model.h>
#include <tqueue.h>

using namespace std;

struct OPSPriv {
	IModel *model;
	vector<Node *> order;
};

struct OrderedWorker : public QRunnable {
	OrderedWorker(OPSPriv *pd, Node *last, int time, int dt);
	void run();
	void updatePorts(Node *sink);

	tqueue<Node *> *in;
	tqueue<Node *> *out;

	OPSPriv *pd;
	Node *last;
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
	std::cout << "thread count: " << pool->maxThreadCount() << std::endl;
	//pool->setMaxThreadCount(1);
	tqueue<Node *> first;

	vector<Node *> order = getOrder();

	BOOST_FOREACH(Node *n, order) {
		first.enqueue(n);
	}

	tqueue<Node *> *upper_queue = &first;
	Node *last = order.back();

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
	vector<Node*> order;
	while (!sources.empty()) {
		Node *n = sources.front();
		sources.pop();
		order.push_back(n);

		while (true) {
			if (pd->model->forward(n).size() == 1) {
				Node *tmp = pd->model->forward(n)[0].get<1>();
				order.push_back(tmp);
				n = tmp;
				continue;
			}
			BOOST_FOREACH(next_node_type next, pd->model->forward(n)) {
				sources.push(next.get<1>());
			}
			break;
		}
	}
	cd3assert(order.size() == pd->model->getNodes()->size(), "order calc wrong size of nodes");
	return order;
}

void OrderedPipeSimulation::setModel(IModel *model) {
	cd3assert(model, "model null");
	ISimulation::setModel(model);
	pd->model = model;
	pd->order = getOrder();
}

OrderedWorker::OrderedWorker(OPSPriv *pd, Node *last, int time, int dt) {
	this->pd = pd;
	this->last = last;
	this->time = time;
	this->dt = dt;
	out = new tqueue<Node *>();//TODO leak
}

void OrderedWorker::run() {
	std::cout << time << "| starting worker " <<  std::endl;
	Node *current;
	do {
		current = in->dequeue();
		std::cout << time << "| got node " << current << std::endl;
		updatePorts(current);
		current->f(time, dt);
		out->enqueue(current);
	} while (current != last);
	std::cout << time << "| stopping worker " <<  std::endl;
}

void OrderedWorker::updatePorts(Node *sink) {
	BOOST_FOREACH(next_node_type con, pd->model->backward(sink)) {
		Node *source;
		std::string src_port, snk_port;
		tie(src_port, source, snk_port) = con;
		sink->setInPort(snk_port, source->getOutPort(src_port));
	}
}
