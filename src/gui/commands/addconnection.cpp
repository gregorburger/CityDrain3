#include "addconnection.h"
#include <node.h>
#include <simulationscene.h>

#define fstd(qs) (QString::fromStdString(qs))

AddConnection::AddConnection(SimulationScene *scene, ConnectionItem *item)
	: DeleteConnection(scene, item) {

}

void AddConnection::undo() {
	DeleteConnection::redo();
}

void AddConnection::redo() {
	static bool first = true;
	if (first) {
		first = false;
		return;
	}
	DeleteConnection::undo();
}
