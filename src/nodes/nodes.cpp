#include "constsource.h"
#include "fileout.h"
#include "mixer.h"
#include "defaultsimulation.h"
#include "vardtsimulation.h"
#ifndef OPENMP_DISABLED
#include "parallelsimulation.h"
#include "pipelinedsimulation.h"
#include "orderedpipesimulation.h"
#endif
#include "sewer.h"
#include "testnode.h"
#include "cso.h"
#include "splitter.h"
#include "catchmentcss.h"
#include "null.h"
#include "cyclenodestart.h"
#include "cyclenodeend.h"
#include "arrayparamtest.h"
#include "ixxrainread.h"
#include "pumpingstation.h"
#include "dwf.h"
#include "wwtp.h"
#include "simplepump.h"
#include "impuls.h"
#include "simplemixer.h"
#include "reactornode.h"
#include "river.h"
#include "ixxqread.h"
#include "seasonqread.h"

#include <noderegistry.h>
#include <nodefactory.h>
#include <simulationregistry.h>
#include <simulationfactory.h>

#include <typefactory.h>
#include <cd3globals.h>

static const char *SOURCE = "Default Nodes";

extern "C" {
	void CD3_PUBLIC registerNodes(NodeRegistry *registry) {
		registry->addNodeFactory(new NodeFactory<ConstSource>(SOURCE));
		registry->addNodeFactory(new NodeFactory<FileOut>(SOURCE));
		registry->addNodeFactory(new NodeFactory<Mixer>(SOURCE));
		registry->addNodeFactory(new NodeFactory<Sewer>(SOURCE));
		registry->addNodeFactory(new NodeFactory<TestNode>(SOURCE));
		registry->addNodeFactory(new NodeFactory<CSO>(SOURCE));
		registry->addNodeFactory(new NodeFactory<Splitter>(SOURCE));
		registry->addNodeFactory(new NodeFactory<CatchmentCSS>(SOURCE));
		registry->addNodeFactory(new NodeFactory<Null>(SOURCE));
		registry->addNodeFactory(new NodeFactory<CycleNodeEnd>(SOURCE));
		registry->addNodeFactory(new NodeFactory<CycleNodeStart>(SOURCE));
		registry->addNodeFactory(new NodeFactory<ArrayParamTest>(SOURCE));
		registry->addNodeFactory(new NodeFactory<IxxRainRead>(SOURCE));
		registry->addNodeFactory(new NodeFactory<PumpingStation>(SOURCE));
		registry->addNodeFactory(new NodeFactory<DWF>(SOURCE));
		registry->addNodeFactory(new NodeFactory<WWTP>(SOURCE));
		registry->addNodeFactory(new NodeFactory<SimplePump>(SOURCE));
		registry->addNodeFactory(new NodeFactory<Impuls>(SOURCE));
		registry->addNodeFactory(new NodeFactory<SimpleMixer>(SOURCE));
		registry->addNodeFactory(new NodeFactory<ReactorNode>(SOURCE));
		registry->addNodeFactory(new NodeFactory<River>(SOURCE));
		registry->addNodeFactory(new NodeFactory<IxxQRead>(SOURCE));
		registry->addNodeFactory(new NodeFactory<SeasonQRead>(SOURCE));
	}

	void CD3_PUBLIC registerSimulations(SimulationRegistry *registry) {
		registry->addSimulationFactory(new SimulationFactory<DefaultSimulation>());
		//registry->addSimulationFactory(new SimulationFactory<VarDTSimulation>());
#ifndef OPENMP_DISABLED
                registry->addSimulationFactory(new SimulationFactory<ParallelSimulation>());
		registry->addSimulationFactory(new SimulationFactory<PipelinedSimulation>());
                registry->addSimulationFactory(new SimulationFactory<OrderedPipeSimulation>());
#endif
	}
}
