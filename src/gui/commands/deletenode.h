#ifndef DELETENODE_H
#define DELETENODE_H

#include <QUndoCommand>
#include <QMap>
#include <QPointF>
#include <string>

class SimulationScene;
class NodeItem;

class DeleteNode : public QUndoCommand
{
public:
	DeleteNode(SimulationScene *scene, NodeItem *item);
	virtual ~DeleteNode() {}
	virtual void undo();
	virtual void redo();
private:
	SimulationScene *scene;
	std::string node_id;
	std::string node_class;
	QMap<std::string, std::string> parameters;
	QPointF pos;
};

#endif // DELETENODE_H
