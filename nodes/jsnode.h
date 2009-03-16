#ifndef JSNODE_H
#define JSNODE_H

#include <node.h>
#include <v8.h>

#include "jsbindings.h"

using namespace v8;

class JSNodePrivate;
class Flow;

CD3_DECLARE_NODE(JSNode)
public:
	///
	JSNode(const std::string &s){(void) s;}
	JSNode();
	virtual ~JSNode();
	void init(int start, int end, int dt);
	void deinit();
	int f(int time, int dt);

private:
	Handle<String> loadScript() const;
	void runScriptInit(int start, int stop, int dt);
	void setJSFunctions(Handle<ObjectTemplate> global);
	void doAddInPort(const char *string, Flow *f);
	void doAddOutPort(const char *string, Flow *f);
private:
	std::string script_path;
	JSNodePrivate *priv;

	friend Handle<Value> jsAddInPort(const Arguments &args);
	friend Handle<Value> jsAddOutPort(const Arguments &args);
};

#endif // JSNODE_H
