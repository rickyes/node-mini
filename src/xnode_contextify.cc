#include "xnode.h"
#include "xnode_contextify.h"

namespace xnode {

namespace contextify {

void ContextScript::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
    printf("hello module");
}

void ContextScript::Init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::String> class_name = v8::String::NewFromUtf8(isolate, "ContextifyScript");
    v8::Local<v8::FunctionTemplate> script_tmpl = v8::FunctionTemplate::New(isolate);
    script_tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    script_tmpl->SetClassName(class_name);
    NODE_SET_PROTOTYPE_METHOD(script_tmpl, "sayModule", sayModule);
}

void ContextScript::sayModule(const v8::FunctionCallbackInfo<v8::Value>& args) {
    printf("hi sayModule");
}
void Initialize(v8::Local<v8::Object> target,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context) {
    ContextScript::Init(context->GetIsolate(), target);
}

}
}

XNODE_BUILTIN_MODULE_CONTEXT_AWARE(contextify, xnode::contextify::Initialize)
