#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

class Node;

typedef std::map<std::string, boost::shared_ptr<Node> > node_map;

class Model
{
public:
    Model();
	~Model();
	void addNode(const std::string &name, boost::shared_ptr<Node> node);
	void dump();

private:
	node_map nodes;
};

#endif // MODEL_H
