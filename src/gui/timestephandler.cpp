#include "timestephandler.h"
#include <simulation.h>

TimeStepHandler::TimeStepHandler() {
	old_progress = -1;
}

TimeStepHandler::~TimeStepHandler() {
}

void TimeStepHandler::after(ISimulation *sim, ptime t) {
	SimulationParameters sp = sim->getSimulationParameters();
	long totsecs = time_period(sp.start + seconds(sp.dt), sp.stop).length().total_seconds();
	long cursecs = time_period(sp.start + seconds(sp.dt), t).length().total_seconds();
	int cur_prog = qRound(100.0 / totsecs * cursecs);
	if (old_progress != cur_prog) {
		old_progress = cur_prog;
		Q_EMIT(progress(cur_prog));
	}
	if (sp.stop == t) {
		Q_EMIT(finished());
	}
}
