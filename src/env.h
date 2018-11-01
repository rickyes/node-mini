#ifndef __ENV_H__
#define __ENV_H__

#include "v8.h"

class Environment {
    public:
        static v8::Isolate* getCurrent(const v8::FunctionCallbackInfo<v8::Value>& args);
};

#endif