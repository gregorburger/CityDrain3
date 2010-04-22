#include "changeparameters.h"
#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <node.h>

ChangeParameters::ChangeParameters(SimulationScene *scene,
								   NodeItem *node,
								   SavedParameters before,
								   std::string id_before)
	: scene(scene),
	before(before), after(node->saveParameters()),
	id_before(id_before), id_after(node->getId().toStdString()), first_redo(true) {

}

ChangeParameters::~ChangeParameters() {
}

void ChangeParameters::undo() {
	QString qid_after = QString::fromStdString(id_after);
	QString qid_before = QString::fromStdString(id_before);
	NodeItem *item = scene->findItem(qid_after);
	Q_ASSERT(item != 0);
	scene->model->renameNode(item->getNode(), id_before);
	scene->renameNodeItem(qid_after, qid_before);
	item->restoreParameters(before);
	SimulationParameters sp = scene->simulation->getSimulationParameters();
	item->getNode()->init(sp.start, sp.stop, sp.dt);
	item->updatePorts();
	scene->update();
}

void ChangeParameters::redo() {
	if (first_redo) {
		first_redo = false;
		return;
	}
	QString qid_after = QString::fromStdString(id_after);
	QString qid_before = QString::fromStdString(id_before);
	NodeItem *item = scene->findItem(qid_before);
	Q_ASSERT(item != 0);
	scene->model->renameNode(item->getNode(), id_after);
	scene->renameNodeItem(qid_before, qid_after);
	item->restoreParameters(after);
	SimulationParameters sp = scene->simulation->getSimulationParameters();
	item->getNode()->init(sp.start, sp.stop, sp.dt);
	item->updatePorts();
	scene->update();
}
