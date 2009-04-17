#include "pipelinedsimulation.h"
#include <model.h>
#include <boost/foreach.hpp>
#include <node.h>

CD3_DECLARE_SIMULATION_NAME(PipelinedSimulation);

PipelinedSimulation::PipelinedSimulation() {
}

PipelinedSimulation::~PipelinedSimulation() {
}

void PipelinedSimulation::start(int time) {
	static double one_perc = 1.0 / sim_param.stop;
	static int old_perc_progress = 0;

	current_time = time;
#ifdef _OPENMP
#pragma omp parallel
#endif
	for (current_time = time; current_time <= sim_param.stop; ) {
	//while (current_time <= sim_param.stop) {
		int percent = static_cast<int>(one_perc * current_time * 100);
		if (percent != old_perc_progress) {
			old_perc_progress = percent;
			//progress(percent);
		}
		current_time += run(current_time, sim_param.dt);
		//timestep(this, current_time);
	}
	/*if (old_perc_progress != 100)
		progress(100);*/
}

int PipelinedSimulation::run(int time, int dt) {
	static bool first = true;

	if (first) {
		node_set_type set_sources = model->getSourceNodes();
		sources = std::vector<Node*>(set_sources.begin(), set_sources.end());
		first = false;
	}

	std::map<Node *, int> deps = createDependsMap();
	int sources_size = sources.size();
	for (int i = 0; i < sources_size; i++) {
		Node *n = sources.at(i);
		run(n, time, deps);
	}
	return dt;
}

void PipelinedSimulation::run(Node *n, int time, std::map<Node *, int> &depends) {
#ifdef _OPENMP
#pragma omp ordered
#endif
	n->f(time, sim_param.dt);

	std::vector<next_node_type> fwd = model->forward(n);
	int fwd_sizes = fwd.size();
	for (int i = 0; i < fwd_sizes; i++) {
		next_node_type con = fwd.at(i);
		std::string src_port, snk_port;
		Node *next;
		boost::tuples::tie(src_port, next, snk_port) = con;

		next->setInPort(snk_port, n->getOutPort(src_port));
#ifdef _OPENMP
#pragma omp atomic
#endif
		depends[next]--;

		if (depends[next] > 0) {
			return;
		}
		run(next, time, depends);
	}
	return;
}

std::map<Node *, int> PipelinedSimulation::createDependsMap() const {
	std::map<Node *, int> deps;
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		deps[node] = model->backward(node).size();
	}

	return deps;
}

