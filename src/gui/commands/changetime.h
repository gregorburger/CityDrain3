#ifndef CHANGETIME_H
#define CHANGETIME_H

#include <QUndoCommand>
#include <simulation.h>

class SimulationScene;

class ChangeTime : public QUndoCommand {
public:
	ChangeTime(SimulationScene *scene,
			   const SimulationParameters &before,
			   const SimulationParameters &after);

	virtual ~ChangeTime() {}
	virtual void undo();
	virtual void redo();

private:
	SimulationScene *scene;
	SimulationParameters before, after;
	bool first_redo;
};

#endif // CHANGETIME_H
