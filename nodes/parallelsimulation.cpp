#include "parallelsimulation.h"
#include <model.h>
#include <node.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>
#ifdef _OPENMP
#include <omp.h>
#endif

CD3_DECLARE_SIMULATION_NAME(ParallelSimulation)

ParallelSimulation::ParallelSimulation() {
#ifdef _OPENMP
	std::cout << "number of threads: " << omp_get_num_threads() << " of " <<  omp_get_max_threads() <<  std::endl;
#endif
}

ParallelSimulation::~ParallelSimulation() {
}

int ParallelSimulation::run(int time, int dt) {
	unordered_map<Node *, int> deps = createDependsMap();
	int sources_size = sources.size();
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < sources_size; i++) {
		Node *n = sources.at(i);
		run(n, time, deps);
	}
	return dt;
}


void ParallelSimulation::run(Node *n, int time, unordered_map<Node *, int> &depends) {
	n->f(time, sim_param.dt);
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		con->pushDirect();
		Node *next = con->sink;

		bool finished;
//#pragma omp atomic
		depends[next]--;
		finished = depends[next] == 0;

		cd3assert(depends[next] >= 0, "race condition");
		if (!finished) {
			return;
		}

		run(next, time, depends);
	}
}

void ParallelSimulation::setModel(IModel *model) {
	this->model = model;

	node_set_type set_sources = model->getSourceNodes();
	sources = std::vector<Node*>(set_sources.begin(), set_sources.end());
}

unordered_map<Node *, int> ParallelSimulation::createDependsMap() const {
	unordered_map<Node *, int> deps;
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		deps[node] = model->backward(node).size();
	}

	return deps;
}
