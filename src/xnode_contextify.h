#ifndef __XNODE_CONTEXTIZFY_H__
#define __XNODE_CONTEXTIZFY_H__

// #if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "v8.h"
#include "xnode_binding.h"

namespace  xnode {
namespace contextify {
class ContextScript {
    public:
        static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
    private:
        explicit ContextScript(v8::Isolate* isolate, v8::Local<v8::Object> object);
        ~ContextScript();
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void sayModule(const v8::FunctionCallbackInfo<v8::Value>& args);
        static v8::Persistent<v8::Function> constructor;
};

}
}

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

// #endif  // __XNODE_CONTEXTIZFY_H__