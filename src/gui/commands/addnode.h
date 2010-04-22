#ifndef ADDNODE_H
#define ADDNODE_H

#include "deletenode.h"

class AddNode : public DeleteNode {
public:
	AddNode(SimulationScene *scene, NodeItem *item);
	virtual ~AddNode() {}
	void undo();
	void redo();
private:
	bool first;
};

#endif // ADDNODE_H
