#ifndef JSBINDINGS_H
#define JSBINDINGS_H

#include <v8.h>
#include "v8cppwrapper.h"
class Flow;
class Test;

v8::Handle<v8::Value> print(const v8::Arguments &args);
v8::CppWrapper<Flow> *bindFlow();

v8::Handle<v8::Value> jsAddInPort(const v8::Arguments &args);
v8::Handle<v8::Value> jsAddOutPort(const v8::Arguments &args);


#endif // JSBINDINGS_H
