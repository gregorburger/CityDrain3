#ifndef MODULE_H
#define MODULE_H

#include <string>

using namespace std;
class NodeRegistry;
#ifdef __cplusplus
extern "C" {
#endif


void initcd3();
void registerPythonNodes(NodeRegistry *registry, const string &module);

#ifdef __cplusplus
}
#endif
#endif // MODULE_H
