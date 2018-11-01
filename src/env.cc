#include "env.h"

v8::Isolate* Environment::getCurrent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    return args.GetIsolate();
};