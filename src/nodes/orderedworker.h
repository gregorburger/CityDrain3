#ifndef ORDEREDWORKER_H
#define ORDEREDWORKER_H

#include <QRunnable>
#include "orderedpipesimulation.h"

struct OrderedWorker : public QRunnable {
	OrderedWorker(OrderedPipeSimulation *sim,
				  Node *last,
				  ptime time, int dt);
	void run();
	void pullPorts(Node *sink);
	void pushPorts(Node *sink);

	sh_node_queue in;
	sh_node_queue out;

	OrderedPipeSimulation *sim;
	Node *last;
	ptime time;
	int dt;
};

#endif // ORDEREDWORKER_H
