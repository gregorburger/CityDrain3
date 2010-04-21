#include "addnode.h"

AddNode::AddNode(SimulationScene *scene, NodeItem *item)
	: DeleteNode(scene, item) {
}

void AddNode::redo() {
	static bool first = true;
	if (first) {
		first = false;
		return;
	}
	DeleteNode::undo();
}

void AddNode::undo() {
	DeleteNode::redo();
}
