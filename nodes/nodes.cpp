#include "constsource.h"
#include "flowtypefactory.h"
#include "fileout.h"
#include "mixer.h"

#include <noderegistry.h>
#include <nodefactory.h>
#include <typeregistry.h>
#include <typefactory.h>


extern "C" {
	void registerNodes(INodeRegistry *registry) {
		registry->doAddNodeFactory(new NodeFactory<ConstSource>());
		registry->doAddNodeFactory(new NodeFactory<FileOut>());
		registry->doAddNodeFactory(new NodeFactory<Mixer>());
	}

	void registerTypes(TypeRegistry *registry) {
		registry->addTypeFactory<Flow>(new FlowTypeFactory());
	}
}
