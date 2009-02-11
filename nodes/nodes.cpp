
#include "constsource.h"
#include "flowtypefactory.h"
#include "fileout.h"
#include <noderegistry.h>
#include <nodefactory.h>
#include <typeregistry.h>
#include <typefactory.h>


extern "C" {
	void registerNodes(INodeRegistry *registry) {
		registry->addNodeFactory(boost::shared_ptr<INodeFactory>(new NodeFactory<ConstSource>()));
		registry->addNodeFactory(boost::shared_ptr<INodeFactory>(new NodeFactory<FileOut>()));
	}

	void registerTypes(TypeRegistry *registry) {
		registry->addTypeFactory<Flow>(boost::shared_ptr<ITypeFactory>(new FlowTypeFactory()));
	}
}
