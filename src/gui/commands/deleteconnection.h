#ifndef DELETECONNECTION_H
#define DELETECONNECTION_H

#include <QUndoCommand>
#include <string>

class SimulationScene;
class ConnectionItem;
class NodeItem;

class DeleteConnection : public QUndoCommand {
public:
	DeleteConnection(SimulationScene *scene, ConnectionItem *item);
	virtual ~DeleteConnection() {}

	virtual void undo();
	virtual void redo();

protected:
	SimulationScene *scene;
	std::string source, source_port, sink, sink_port;
};

#endif // DELETECONNECTION_H
