#ifndef MODULE_H
#define MODULE_H

#include <string>

using namespace std;
class NodeRegistry;
#ifdef __cplusplus
extern "C" {
#endif

class Node;

void initcd3();

#ifdef __cplusplus
}

struct PythonEnvPriv;

class PythonEnv {
public:
	virtual ~PythonEnv();
	static PythonEnv *getInstance();
	void registerNodes(NodeRegistry *registry,
					   const string &module);
	Node *createNode(std::string name);
private:
	PythonEnv();
	PythonEnvPriv *priv;
	static PythonEnv *instance;
};

void test_node(Node *n);
#include <boost/python.hpp>
void set_self(boost::python::object o);
#endif
#endif // MODULE_H
