#include "parallelsimulation.h"
#include <model.h>
#include <node.h>
#include <boost/foreach.hpp>
#include <omp.h>

CD3_DECLARE_SIMULATION_NAME(ParallelSimulation)

ParallelSimulation::ParallelSimulation() {
    std::cout << "number of threads: " << omp_get_num_threads() << " of " <<  omp_get_max_threads() <<  std::endl;
}

ParallelSimulation::~ParallelSimulation() {
}

int ParallelSimulation::run(int time, int dt) {
	static bool first = true;

	if (first) {
	    node_set_type set_sources = model->getSourceNodes();
	    sources = std::vector<Node*>(set_sources.begin(), set_sources.end());
	    first = false;
	}

        std::tr1::unordered_map<Node *, int> deps = createDependsMap();
	int sources_size = sources.size();
#pragma omp parallel for
	for (int i = 0; i < sources_size; i++) {
		Node *n = sources.at(i);
		run(n, time, deps);
	}
	return dt;
}


void ParallelSimulation::run(Node *n, int time, std::tr1::unordered_map<Node *, int> &depends) {
	n->f(time, sim_param.dt);
	std::vector<next_node_type> fwd = model->forward(n);
	int fwd_sizes = fwd.size();
	for (int i = 0; i < fwd_sizes; i++) {
		next_node_type con = fwd.at(i);
		std::string src_port, snk_port;
		Node *next;
		boost::tuples::tie(src_port, next, snk_port) = con;

		next->setInPort(snk_port, n->getOutPort(src_port));

#pragma omp atomic
		depends[next]--;

		if (depends[next] > 0) {
			return;
		}
		run(next, time, depends);
	}
	return;
}

std::tr1::unordered_map<Node *, int> ParallelSimulation::createDependsMap() const {
        std::tr1::unordered_map<Node *, int> deps;
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		deps[node] = model->backward(node).size();
	}

	return deps;
}
