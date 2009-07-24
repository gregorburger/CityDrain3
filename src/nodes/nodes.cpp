#include "constsource.h"
#include "fileout.h"
#include "mixer.h"
#include "defaultsimulation.h"
#include "vardtsimulation.h"
#include "parallelsimulation.h"
#include "pipelinedsimulation.h"
#include "orderedpipesimulation.h"
#include "sewer.h"
#include "rainread.h"
#include "testnode.h"
#include "qs/qswnode.h"
#include "cso.h"
#include "splitter.h"
#include "catchmentcss.h"
#include "null.h"
#include "cyclenodestart.h"
#include "cyclenodeend.h"

#include <noderegistry.h>
#include <nodefactory.h>
#include <typeregistry.h>
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
		registry->addNodeFactory(new NodeFactory<RainRead>());
		registry->addNodeFactory(new NodeFactory<TestNode>());
		registry->addNodeFactory(new NodeFactory<QSWNode, true>());
		registry->addNodeFactory(new NodeFactory<CSO>());
		registry->addNodeFactory(new NodeFactory<Splitter>());
		registry->addNodeFactory(new NodeFactory<CatchmentCSS>());
		registry->addNodeFactory(new NodeFactory<Null>());
		registry->addNodeFactory(new NodeFactory<CycleNodeEnd>());
		registry->addNodeFactory(new NodeFactory<CycleNodeStart>());
	}

	void CD3_PUBLIC registerTypes(TypeRegistry *registry) {
		(void) registry;
	}

	void CD3_PUBLIC registerSimulations(SimulationRegistry *registry) {
		registry->addSimulationFactory(new SimulationFactory<DefaultSimulation>());
		//registry->addSimulationFactory(new SimulationFactory<VarDTSimulation>());
		registry->addSimulationFactory(new SimulationFactory<ParallelSimulation>());
		registry->addSimulationFactory(new SimulationFactory<PipelinedSimulation>());
		registry->addSimulationFactory(new SimulationFactory<OrderedPipeSimulation>());
	}
}
