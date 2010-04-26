#include "renamenode.h"
#include <simulationscene.h>
#include <connectionitem.h>
#include <mapbasedmodel.h>

RenameNode::RenameNode(SimulationScene *scene, QString before, QString after)
	: scene(scene), before(before), after(after) {
}

void RenameNode::undo() {
	rename(after, before);
	//scene->update();
}

void RenameNode::redo() {
	rename(before, after);
	//scene->update();
}

void RenameNode::rename(QString old_id, QString new_id) {
	if (old_id == new_id) {
		return;
	}

	NodeItem *item = scene->node_items[old_id];
	Q_ASSERT(item);

	scene->getModel()->renameNode(item->getNode(), new_id.toStdString());
	scene->node_items.remove(old_id);
	scene->node_items[new_id] = item;

	QList<ConnectionItem*> cons = scene->connections_of_node.values(old_id);

	scene->connections_of_node.remove(old_id);
	Q_FOREACH(ConnectionItem *citem, cons) {
		if (citem->getSourceId() == old_id) {
			citem->setSourceId(new_id);
		}
		if (citem->getSinkId() == old_id) {
			citem->setSinkId(new_id);
		}
		scene->connections_of_node.insert(new_id, citem);
	}
	item->updatePorts();//does updateBoundingBox too
	scene->updateConnections(item);
}
