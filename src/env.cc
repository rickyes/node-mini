#include "v8.h"
#include "env.h"

using v8::HandleScope;
using v8::Isolate;
using v8::FunctionTemplate;
using v8::Object;
using v8::Local;
using v8::Context;


namespace xnode {

 Environment::Environment(v8::Local<v8::Context> context): isolate_(context->GetIsolate()) {
    printf("init env");
 }

 void Environment::CreateProperties() {
  HandleScope handle_scope(isolate_);
  Local<Context> ctx = context();
  Local<FunctionTemplate> templ = FunctionTemplate::New(isolate());
  templ->InstanceTemplate()->SetInternalFieldCount(1);
  Local<Object> obj = templ->GetFunction(ctx)
                          .ToLocalChecked()
                          ->NewInstance(ctx)
                          .ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, this);
  set_as_callback_data(obj);
  set_as_callback_data_template(templ);
 }

} // xnode