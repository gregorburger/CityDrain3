#include "nodemove.h"
#include "../nodeitem.h"

NodeMove::NodeMove(NodeItem *item, QPointF old, QPointF _new)
	: QUndoCommand("Node moved"), item(item), old(old), _new(_new) {

}

NodeMove::~NodeMove() {
	qDebug() << "~NodeMove";
}

void NodeMove::undo() {
	qDebug() << "undo";
	item->setPos(old);
}

void NodeMove::redo() {
	qDebug() << "redo";
	item->setPos(_new);
}

bool NodeMove::mergeWith(const QUndoCommand *other) {
	const NodeMove *other_move = (const NodeMove*) other;
	if (other->id() == id() && other_move->item == item) {
		this->_new = other_move->_new;
		return true;
	}
	return false;
}
