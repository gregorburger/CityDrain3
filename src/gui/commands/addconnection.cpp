#include "addconnection.h"
#include <node.h>
#include <simulationscene.h>

#define fstd(qs) (QString::fromStdString(qs))

AddConnection::AddConnection(SimulationScene *scene, ConnectionItem *item)
	: DeleteConnection(scene, item), first(true) {

}

void AddConnection::undo() {
	DeleteConnection::redo();
}

void AddConnection::redo() {
	if (first) {
		first = false;
		return;
	}
	DeleteConnection::undo();
}
