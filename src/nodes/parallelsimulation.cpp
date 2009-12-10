#include "parallelsimulation.h"
#include <imodel.h>
#include <node.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>
#ifdef _OPENMP
#include <omp.h>
#endif

CD3_DECLARE_SIMULATION_NAME(ParallelSimulation)

ParallelSimulation::ParallelSimulation() {
#ifdef _OPENMP
	Logger(Standard) << "number of threads: " << omp_get_num_threads() << " of " <<  omp_get_max_threads();
#endif
}

ParallelSimulation::~ParallelSimulation() {
}

int ParallelSimulation::run(int time, int dt) {
	setNodeDepends();
	int sources_size = sources.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
#endif
	for (int i = 0; i < sources_size; i++) {
		shared_ptr<Node> n = sources.at(i);
		run(n, time);
	}

	return dt;
}


void ParallelSimulation::run(shared_ptr<Node> n, int time) {
	n->f(time, sim_param.dt);
	BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
		con->push(sim_param.dt);
		shared_ptr<Node> next = con->sink;

//#pragma omp atomic
		next->num_inputed--;

		cd3assert(next->num_inputed >= 0, "race condition");
		if (next->num_inputed > 0) {
			return;
		}

		run(next, time);
	}
}

void ParallelSimulation::setModel(IModel *model) {
	cd3assert(model, "model null");
	ISimulation::setModel(model);

	node_set_type set_sources = model->getSourceNodes();
	sources = std::vector<shared_ptr<Node> >(set_sources.begin(), set_sources.end());
}

void ParallelSimulation::setNodeDepends() const {
	BOOST_FOREACH(shared_ptr<Node> node, *model->getNodes()) {
		node->num_inputed = model->backwardConnection(node).size();
	}
}
