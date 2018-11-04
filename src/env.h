#ifndef __ENV_H__
#define __ENV_H__

#include "v8.h"

namespace xnode {

class Environment {
    public:
        static inline Environment* getCurrent(const v8::FunctionCallbackInfo<v8::Value>& args);
        inline v8::Isolate* isolate() const;
        inline v8::Local<v8::Context> context() const;
};

} // xnode 

#endif