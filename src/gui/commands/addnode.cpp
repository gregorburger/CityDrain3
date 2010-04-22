#include "addnode.h"

AddNode::AddNode(SimulationScene *scene, NodeItem *item)
	: DeleteNode(scene, item), first(true) {
}

void AddNode::redo() {
	if (first) {
		first = false;
		return;
	}
	DeleteNode::undo();
}

void AddNode::undo() {
	DeleteNode::redo();
}
