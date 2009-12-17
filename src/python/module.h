#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <cd3globals.h>

using namespace std;
class NodeRegistry;
#ifdef __cplusplus
extern "C" {
#endif

class Node;

#ifdef __cplusplus
}

struct PythonEnvPriv;

class CD3_PUBLIC PythonEnv {
public:
	virtual ~PythonEnv();
	static PythonEnv *getInstance();
	static void freeInstance();
	void registerNodes(NodeRegistry *registry,
					   const string &module);
private:
	PythonEnv();
	PythonEnvPriv *priv;
	static PythonEnv *instance;
};

void test_node(Node *n);
#endif
#endif // MODULE_H
