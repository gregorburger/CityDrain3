#ifndef NODEMOVE_H
#define NODEMOVE_H

#include <QUndoCommand>
#include <QPointF>
#include <QDebug>

class NodeItem;

class NodeMove : public QUndoCommand {
public:
	NodeMove(NodeItem *item, QPointF old, QPointF _new);
	virtual ~NodeMove();
	void undo();
	void redo();
	int id() const {
		return 1;
	}

	virtual bool mergeWith(const QUndoCommand *other);

private:
	NodeItem *item;
	QPointF old, _new;
};

#endif // NODEMOVE_H
