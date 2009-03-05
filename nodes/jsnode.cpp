#include "jsnode.h"
#include "jsbindings.h"

#include <fstream>
#include <sstream>
#include <flow.h>
#include <cd3assert.h>
#include "v8cppwrapper.h"

struct JSNodePrivate {
	Handle<Script> script;
	Persistent<Context> context;
	CppWrapper<Test> *test_wrapper;
	CppWrapper<Flow> *flow_wrapper;
	CppWrapper<Node> *node_wrapper;
};

JSNode::JSNode() {
	priv = new JSNodePrivate();
	addParameter(ADD_PARAMETERS(script_path));
	//in_out = new Flow();
	/*addInPort("in", in_out);
	addOutPort("out", in_out);*/
}

JSNode::~JSNode() {
	delete priv;
	//delete in_out;
}

void JSNode::setJSFunctions(Handle<ObjectTemplate> globals) {
	globals->Set(String::New("print"), FunctionTemplate::New(print));
	priv->test_wrapper = bindTest();
	globals->Set(String::New("Test"), priv->test_wrapper->NewFunctionTemplate());
	priv->flow_wrapper = bindFlow();
	globals->Set(String::New("Flow"), priv->flow_wrapper->NewFunctionTemplate());
	globals->Set(String::New("addInPort"), FunctionTemplate::New(jsAddInPort));
	globals->Set(String::New("addOutPort"), FunctionTemplate::New(jsAddOutPort));
	//globals->Set(String::New("test"), FunctionTemplate::New(test));
}

void JSNode::doAddInPort(const char *name, Flow *f) {
	std::string stdname(name);
	addInPort(stdname, f);
}

void JSNode::doAddOutPort(const char *name, Flow *f) {
	std::string stdname(name);
	addOutPort(stdname, f);
}

void JSNode::init(int start, int stop, int dt) {
	HandleScope handle_scope;

	Handle<ObjectTemplate> globals = ObjectTemplate::New();
	globals->SetInternalFieldCount(1);

	setJSFunctions(globals);

	priv->context = Context::New(NULL, globals);
	Context::Scope scope(priv->context);

	Local<Object> proto = Local<Object>::Cast(priv->context->Global()->GetPrototype());
	proto->SetInternalField(0, External::New(this));

	priv->script = Script::Compile(loadScript());
	priv->script->Run();
	runScriptInit(start, stop, dt);
}

void JSNode::deinit() {
	delete priv->flow_wrapper;
	delete priv->test_wrapper;
}

void JSNode::runScriptInit(int start, int stop, int dt) {
	HandleScope handle_scope;
	Local<Object> global = priv->context->Global();
	Handle<Value> init_val = global->Get(String::New("init"));
	assert(init_val->IsFunction(), "no init function defined");
	Handle<Function> init_fun = Handle<Function>::Cast(init_val);
	Handle<Value> args[3];
	args[0] = Integer::New(start);
	args[1] = Integer::New(stop);
	args[2] = Integer::New(dt);
	init_fun->Call(global, 3, args);
}

Handle<String> JSNode::loadScript() const {
	std::ifstream fin(script_path.c_str());
	assert(fin, "could not open script file");
	std::ostringstream os;
	os << fin.rdbuf();

	return String::New(os.str().c_str());
}

int JSNode::f(int time, int dt) {
	(void) time;

	HandleScope handle_scope;

	Context::Scope scope(priv->context);

	Local<Object> global = priv->context->Global();
	Handle<Value> f_val = global->Get(String::New("f"));
	assert(f_val->IsFunction(), "no 'f' function defined");
	Handle<Function> f_fun = Handle<Function>::Cast(f_val);
	Handle<Value> args[2];
	args[0] = Integer::New(time);
	args[1] = Integer::New(dt);

	Handle<Value> ret = f_fun->Call(global, 2, args);
	int dtret = ret->Int32Value();
	return dtret;
}
