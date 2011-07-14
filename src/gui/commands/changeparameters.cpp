#include "changeparameters.h"
#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <node.h>

ChangeParameters::ChangeParameters(SimulationScene *scene,
								   NodeItem *node,
								   SavedParameters before)
	: scene(scene),
	before(before), after(node->saveParameters()),
	id(node->getId()), first_redo(true) {

}

ChangeParameters::~ChangeParameters() {
}

void ChangeParameters::undo() {
	NodeItem *item = scene->findItem(id);
	Q_ASSERT(item != 0);
	item->getNode()->deinit();
	item->restoreParameters(before);
	SimulationParameters sp = scene->simulation->getSimulationParameters();
	item->getNode()->init(sp.start + seconds(sp.dt), sp.stop, sp.dt);
	item->updatePorts();
	//scene->update();
	scene->updateConnections(item);
}

void ChangeParameters::redo() {
	if (first_redo) {
		first_redo = false;
		return;
	}
	NodeItem *item = scene->findItem(id);
	Q_ASSERT(item != 0);
	item->getNode()->deinit();
	item->restoreParameters(after);
	SimulationParameters sp = scene->simulation->getSimulationParameters();
	item->getNode()->init(sp.start + seconds(sp.dt), sp.stop, sp.dt);
	item->updatePorts();
	//scene->update();
	scene->updateConnections(item);
}
