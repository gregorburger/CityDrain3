#include "nodemove.h"
#include "../nodeitem.h"
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
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void NodeMove::redo() {
	NodeItem *item = scene->findItem(node_id);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
	item->setPos(_new);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

bool NodeMove::mergeWith(const QUndoCommand *other) {
	const NodeMove *other_move = (const NodeMove*) other;
	if (other->id() == id() && other_move->node_id == node_id) {
		this->_new = other_move->_new;
		return true;
	}
	return false;
}
