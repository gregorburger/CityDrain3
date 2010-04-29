#include "constsource.h"
#include "fileout.h"
#include "mixer.h"
#include "defaultsimulation.h"
#include "vardtsimulation.h"
#ifndef OPENMP_DISABLED
#include "parallelsimulation.h"
#endif
#include "pipelinedsimulation.h"
#include "orderedpipesimulation.h"
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

#include <noderegistry.h>
#include <nodefactory.h>
#include <simulationregistry.h>
#include <simulationfactory.h>

#include <typefactory.h>
#include <cd3globals.h>

extern "C" {
	void CD3_PUBLIC registerNodes(NodeRegistry *registry) {
		registry->addNodeFactory(new NodeFactory<ConstSource>());
		registry->addNodeFactory(new NodeFactory<FileOut>());
		registry->addNodeFactory(new NodeFactory<Mixer>());
		registry->addNodeFactory(new NodeFactory<Sewer>());
		registry->addNodeFactory(new NodeFactory<TestNode>());
		registry->addNodeFactory(new NodeFactory<CSO>());
		registry->addNodeFactory(new NodeFactory<Splitter>());
		registry->addNodeFactory(new NodeFactory<CatchmentCSS>());
		registry->addNodeFactory(new NodeFactory<Null>());
		registry->addNodeFactory(new NodeFactory<CycleNodeEnd>());
		registry->addNodeFactory(new NodeFactory<CycleNodeStart>());
		registry->addNodeFactory(new NodeFactory<ArrayParamTest>());
		registry->addNodeFactory(new NodeFactory<IxxRainRead>());
		registry->addNodeFactory(new NodeFactory<PumpingStation>());
		registry->addNodeFactory(new NodeFactory<DWF>());
		registry->addNodeFactory(new NodeFactory<WWTP>());
		registry->addNodeFactory(new NodeFactory<SimplePump>());
		registry->addNodeFactory(new NodeFactory<Impuls>());
	}

	void CD3_PUBLIC registerSimulations(SimulationRegistry *registry) {
		registry->addSimulationFactory(new SimulationFactory<DefaultSimulation>());
		//registry->addSimulationFactory(new SimulationFactory<VarDTSimulation>());
		registry->addSimulationFactory(new SimulationFactory<ParallelSimulation>());
#ifndef OPENMP_DISABLED
		registry->addSimulationFactory(new SimulationFactory<PipelinedSimulation>());
#endif
		registry->addSimulationFactory(new SimulationFactory<OrderedPipeSimulation>());
	}
}
