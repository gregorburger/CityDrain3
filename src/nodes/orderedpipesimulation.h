#ifndef ORDEREDPIPESIMULATION_H
#define ORDEREDPIPESIMULATION_H

#include <vector>

#include <simulation.h>
#include <boost/shared_ptr.hpp>
#include <tqueue.h>

struct NodeConnection;
class Model;
class Node;

typedef boost::shared_ptr<tqueue<Node *> > sh_node_queue;

CD3_DECLARE_SIMULATION(OrderedPipeSimulation)
public:
	OrderedPipeSimulation();
	~OrderedPipeSimulation();
	void start(ptime time);
	int run(ptime time, int dt);
	void setModel(IModel *model);
	NodeConnection *createConnection(Node *, const std::string &,
									 Node *, const std::string &) const;
private:
	std::vector<Node *> getOrder();
	IModel *model;
	std::vector<Node *> order;
};

#endif // ORDEREDPIPESIMULATION_H
