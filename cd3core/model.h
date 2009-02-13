#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <set>

class Node;
class SimulationParameters;


class IModel {
public:
	virtual ~IModel() {}

	virtual void addNode(const std::string &name, Node *node) = 0;
	virtual void addConnection(const std::string &src_node,
					   const std::string &src_port,
					   const std::string &sin_node,
					   const std::string &sin_port) = 0;

	virtual void initNodes(const SimulationParameters &) = 0;
	virtual std::set<Node *> sourceNodes() = 0;
	virtual std::set<Node *> sinkNodes() = 0;

	virtual std::set<Node *> forward(Node *n) = 0;
	virtual std::set<Node *> backward(Node *n) = 0;
};

#endif // MODEL_H
