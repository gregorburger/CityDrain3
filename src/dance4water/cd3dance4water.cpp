#include "household.h"
#include "storage.h"
#include "stormwaterlink.h"
#include "vibemixer.h"
#include "vibesplitter.h"
#include <noderegistry.h>
#include <nodefactory.h>


#include <typefactory.h>
#include <cd3globals.h>

static const char *SOURCE = "DAnCE4Water";

extern "C" {
	void CD3_PUBLIC registerNodes(NodeRegistry *registry) {
                registry->addNodeFactory(new NodeFactory<Household>(SOURCE));
                registry->addNodeFactory(new NodeFactory<Storage>(SOURCE));
                registry->addNodeFactory(new NodeFactory<StormwaterLink>(SOURCE));
                registry->addNodeFactory(new NodeFactory<VIBeMixer>(SOURCE));
                registry->addNodeFactory(new NodeFactory<VIBeSplitter>(SOURCE));

	}

}
