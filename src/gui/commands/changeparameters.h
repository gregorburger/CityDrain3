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
					 SavedParameters before,
					 std::string d_before);
	virtual ~ChangeParameters();
	void redo();
	void undo();
private:
	SimulationScene *scene;
	SavedParameters before, after;
	std::string id_before, id_after;
	bool first_redo;
};

#endif // CHANGEPARAMETERS_H
