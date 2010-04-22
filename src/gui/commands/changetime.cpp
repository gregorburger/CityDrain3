#include "changetime.h"

#include <simulationscene.h>
#include <mapbasedmodel.h>

ChangeTime::ChangeTime(SimulationScene *scene,
					   const SimulationParameters &before,
					   const SimulationParameters &after)
	: scene(scene), before(before), after(after), first_redo(true) {

}

void ChangeTime::redo() {
	if (first_redo) {
		first_redo = false;
		return;
	}
	scene->getModel()->deinitNodes();
	bool empty = scene->getModel()->initNodes(after).empty();
	Q_ASSERT(empty);
	scene->getSimulation()->setSimulationParameters(after);
	scene->simulationParametersChanged();
}

void ChangeTime::undo() {
	scene->getModel()->deinitNodes();
	bool empty = scene->getModel()->initNodes(before).empty();
	Q_ASSERT(empty);
	scene->getSimulation()->setSimulationParameters(before);
	scene->simulationParametersChanged();
}
