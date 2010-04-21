#ifndef ADDCONNECTION_H
#define ADDCONNECTION_H

#include "deleteconnection.h"

class SimulationScene;
class ConnectionItem;

class AddConnection : public DeleteConnection {
public:
	AddConnection(SimulationScene *scene, ConnectionItem *con);
	virtual ~AddConnection() {}
	void redo();
	void undo();
};

#endif // ADDCONNECTION_H
