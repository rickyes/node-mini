#include "v8.h"
#include "xnode_contextify.h"
#include "xnode.h"
#include "env-inl.h"

namespace xnode {

namespace contextify {

using v8::Local;
using v8::Context;
using v8::Object;
using v8::Value;
using v8::Context;
using v8::Persistent;
using v8::Function;
using v8::String;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::FunctionTemplate;
using v8::HandleScope;

Persistent<Function> ContextScript::constructor;

void ContextScript::New(const FunctionCallbackInfo<Value>& args) {
}

static void sayModule(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "this is sayModule").ToLocalChecked());
}

void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void *priv) {

    Isolate* isolate = context->GetIsolate();

    target->Set(
        context,
        String::NewFromUtf8(isolate, "sayModule").ToLocalChecked(),
        FunctionTemplate::New(isolate, sayModule)->GetFunction(context).ToLocalChecked()
    ).Check();
}
}
}


XNODE_BUILTIN_MODULE_CONTEXT_AWARE(contextify, xnode::contextify::Initialize)
