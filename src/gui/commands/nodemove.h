#ifndef NODEMOVE_H
#define NODEMOVE_H

#include <QUndoCommand>
#include <QPointF>

class NodeItem;
class SimulationScene;

class NodeMove : public QUndoCommand {
public:
	NodeMove(SimulationScene *scene, NodeItem *item,
			 QPointF old, QPointF _new);
	virtual ~NodeMove();
	void undo();
	void redo();
	int id() const {
		return 1;
	}

	virtual bool mergeWith(const QUndoCommand *other);

private:
	void updateConnections();
	SimulationScene *scene;
	QPointF old, _new;
	QString node_id;
};

#endif // NODEMOVE_H
