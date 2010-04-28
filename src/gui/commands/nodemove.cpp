#include "nodemove.h"
#include "../nodeitem.h"
#include "../connectionitem.h"
#include <simulationscene.h>

NodeMove::NodeMove(SimulationScene *scene, NodeItem *item, QPointF old, QPointF _new)
	: QUndoCommand("Node moved"), scene(scene),
	old(old), _new(_new), node_id(item->getId()) {

}

NodeMove::~NodeMove() {
}

void NodeMove::undo() {
	NodeItem *item = scene->findItem(node_id);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
	item->setPos(old);
	scene->updateConnections(item);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	//scene->update();
}

void NodeMove::redo() {
	NodeItem *item = scene->findItem(node_id);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
	item->setPos(_new);
	scene->updateConnections(item);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	//scene->update();
}

bool NodeMove::mergeWith(const QUndoCommand *other) {
	const NodeMove *other_move = static_cast<const NodeMove*>(other);
	if (other->id() == id() && other_move->node_id == node_id) {
		this->_new = other_move->_new;
		return true;
	}
	return false;
}
