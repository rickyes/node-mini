#ifndef __XNODE_CONTEXTIZFY_H__
#define __XNODE_CONTEXTIZFY_H__

// #if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "v8.h"
#include "xnode_binding.h"

namespace  xnode {
namespace contextify {
class ContextScript {
    public:
        static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
        static void sayModule(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        ContextScript(v8::Isolate* isolate, v8::Local<v8::Object> object);
        ~ContextScript();
};

}
}

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

// #endif  // __XNODE_CONTEXTIZFY_H__