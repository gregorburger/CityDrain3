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
	NodeConnection *createConnection(shared_ptr<Node> , const std::string &,
									 shared_ptr<Node> , const std::string &) const;
private:
	std::vector<shared_ptr<Node> > getOrder();
	void updatePorts(shared_ptr<Node> sink);
	OPSPriv *pd;
};

#endif // ORDEREDPIPESIMULATION_H
