#ifndef CHANGEPARAMETERS_H
#define CHANGEPARAMETERS_H

#include <QUndoCommand>
#include <QMap>
#include "../nodeitem.h"

class SimulationScene;

class ChangeParameters : public QUndoCommand
{
public:
	ChangeParameters(SimulationScene *scene,
					 NodeItem *node,
					 SavedParameters before);
	virtual ~ChangeParameters();
	void redo();
	void undo();
private:
	SimulationScene *scene;
	SavedParameters before, after;
	QString id;
	bool first_redo;
};

#endif // CHANGEPARAMETERS_H
