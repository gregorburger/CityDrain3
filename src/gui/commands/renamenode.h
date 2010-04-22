#ifndef RENAMENODE_H
#define RENAMENODE_H

#include <QUndoCommand>

class SimulationScene;

class RenameNode : public QUndoCommand {
public:
	RenameNode(SimulationScene *scene, QString before, QString after);
	void undo();
	void redo();
private:
	void rename(QString old_id, QString new_id);
	SimulationScene *scene;
	QString before, after;
};

#endif // RENAMENODE_H
