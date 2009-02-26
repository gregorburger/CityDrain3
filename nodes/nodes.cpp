#include "constsource.h"
#include "flowtypefactory.h"
#include "fileout.h"
#include "mixer.h"
#include "defaultsimulation.h"

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
	}

	void registerTypes(TypeRegistry *registry) {
		registry->addTypeFactory<Flow>(new FlowTypeFactory());
	}

	void registerSimulations(SimulationRegistry *registry) {
		registry->addSimulationFactory(new SimulationFactory<DefaultSimulation>());
	}
}
