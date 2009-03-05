#include "jsbindings.h"


#include <flow.h>
#include "test.h"
#include <calculationunit.h>
#include "jsnode.h"

#include <iostream>

using namespace v8;

#include <cd3assert.h>

Handle<Value> print(const Arguments& args) {
	bool first = true;
	//HandleScope handleScope;
	for (int i = 0; i < args.Length(); ++i) {
		if (first)
			first = false;
		else
			std::cout << ", ";
		std::cout << *String::AsciiValue(args[i]);
	}
	std::cout << std::endl;
	return Undefined();
}


CppWrapper<Flow> *bindFlow() {
	CppWrapper<Flow> *flow_wrapper = new CppWrapper<Flow>();
	/*flow_wrapper.SetFunction<void,
			const std::string &,
			const CalculationUnit *,
			const double>("addUnit", &Flow::addUnit);*/
	flow_wrapper->SetFunction<void>("clear", &Flow::clear);
	flow_wrapper->SetFunction<double, const std::string &>("getValue", &Flow::getValue);

	return flow_wrapper;
}

CppWrapper<Test> *bindTest() {
	CppWrapper<Test> *test_wrapper = new CppWrapper<Test>();
	//test_wrapper.SetAccessor<char *>("test", &Test::getTest, &Test::setTest);
	test_wrapper->SetAccessor<int>("num", &Test::getNum, &Test::setNum);
	return test_wrapper;
}

v8::Handle<v8::Value> jsAddInPort(const v8::Arguments &args) {
	HandleScope hscope;
	assert(args.Length() == 2, "addInPort has 2 arguments");
	assert(args[0]->IsString(), "first argument of addInPort is a String");
	assert(args[1]->IsObject(), "second argument of addInPort is an Object");

	printf("adding in port\n");

	String::AsciiValue port_name(args[0]);
	Handle<External> flow_external = Handle<External>::Cast(args[1]->ToObject()->GetInternalField(0));
	Flow *flow = static_cast<Flow *>(flow_external->Value());

	Handle<Object> holder_proto = Handle<Object>::Cast(args.Holder()->GetPrototype());
	Handle<External> node_external = Handle<External>::Cast(holder_proto->GetInternalField(0));
	JSNode *node = static_cast<JSNode*>(node_external->Value());
	node->doAddInPort(*port_name, flow);
	return Undefined();
}

v8::Handle<v8::Value> jsAddOutPort(const v8::Arguments &args) {
	HandleScope hscope;
	assert(args.Length() == 2, "addOutPort has 2 arguments");
	assert(args[0]->IsString(), "first argument of addOutPort is a String");
	assert(args[1]->IsObject(), "second argument of addOutPort is an Object");

	printf("adding out port\n");

	String::AsciiValue port_name(args[0]);
	Handle<External> flow_external = Handle<External>::Cast(args[1]->ToObject()->GetInternalField(0));
	Flow *flow = static_cast<Flow *>(flow_external->Value());

	Handle<Object> holder_proto = Handle<Object>::Cast(args.Holder()->GetPrototype());
	Handle<External> node_external = Handle<External>::Cast(holder_proto->GetInternalField(0));
	JSNode *node = static_cast<JSNode*>(node_external->Value());
	node->doAddOutPort(*port_name, flow);
	return Undefined();
}
