#ifndef PIPELINEDSIMULATION_H
#define PIPELINEDSIMULATION_H

#include <simulation.h>

struct PipeSimPrivate;
class IModel;

CD3_DECLARE_SIMULATION(PipelinedSimulation)
public:
	PipelinedSimulation();
	virtual ~PipelinedSimulation();
	int run(ptime time, int dt);
	void start(ptime time);
	void setModel(IModel *model);
private:
	PipeSimPrivate *pd;
};

#endif // PIPELINEDSIMULATION_H
