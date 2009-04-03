#include "constsource.h"
#include "flowtypefactory.h"
#include "fileout.h"
#include "mixer.h"
#include "defaultsimulation.h"
#include "vardtsimulation.h"
#include "sewer.h"
#include "rainread.h"
#include "testnode.h"
#include "qs/qswnode.h"
#include "cso.h"
#include "splitter.h"
#include "catchmentcss.h"

#include <noderegistry.h>
#include <nodefactory.h>
#include <typeregistry.h>
#include <simulationregistry.h>
#include <simulationfactory.h>

#include <typefactory.h>


extern "C" {
	void registerNodes(NodeRegistry *registry) {
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
	}

	void registerTypes(TypeRegistry *registry) {
		registry->addTypeFactory<Flow>(new FlowTypeFactory());
	}

	void registerSimulations(SimulationRegistry *registry) {
		registry->addSimulationFactory(new SimulationFactory<DefaultSimulation>());
		registry->addSimulationFactory(new SimulationFactory<VarDTSimulation>());
	}
}
