#ifndef PIPELINEDSIMULATION_H
#define PIPELINEDSIMULATION_H

#include <simulation.h>

CD3_DECLARE_SIMULATION(PipelinedSimulation)
public:
	PipelinedSimulation();
	virtual ~PipelinedSimulation();
	int run(int time, int dt);
	void start(int time);
private:
	std::map<Node *, int> createDependsMap() const;
	void run(Node *n, int time, std::map<Node *, int> &depends);
	std::vector<Node*> sources;
};

#endif // PIPELINEDSIMULATION_H
