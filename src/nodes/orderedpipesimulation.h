#ifndef ORDEREDPIPESIMULATION_H
#define ORDEREDPIPESIMULATION_H

#include <vector>

#include <simulation.h>

struct OPSPriv;
struct NodeConnection;

CD3_DECLARE_SIMULATION(OrderedPipeSimulation)
public:
	OrderedPipeSimulation();
	~OrderedPipeSimulation();
	void start(int time);
	int run(int time, int dt);
	void setModel(IModel *model);
	NodeConnection *createConnection(Node *, const std::string &,
									 Node *, const std::string &) const;
private:
	std::vector<Node*> getOrder();
	void updatePorts(Node *sink);
	OPSPriv *pd;
};

#endif // ORDEREDPIPESIMULATION_H
