
#ifndef SRC_ENV_INL_H_
#define SRC_ENV_INL_H_

#include "env.h"
#include "v8.h"


namespace xnode {

inline v8::Local<v8::FunctionTemplate>
    Environment::NewFunctionTemplate(v8::FunctionCallback callback,
                                     v8::Local<v8::Signature> signature,
                                     v8::ConstructorBehavior behavior,
                                     v8::SideEffectType side_effect_type) {
  v8::Local<v8::Object> external = as_callback_data();
  return v8::FunctionTemplate::New(isolate(), callback, external,
                                   signature, 0, behavior, side_effect_type);
}

inline void Environment::SetMethod(v8::Local<v8::Object> that,
                                   const char* name,
                                   v8::FunctionCallback callback) {
  v8::Local<v8::Context> context = isolate()->GetCurrentContext();
  v8::Local<v8::Function> function =
      NewFunctionTemplate(callback, v8::Local<v8::Signature>(),
                          v8::ConstructorBehavior::kThrow,
                          v8::SideEffectType::kHasSideEffect)
          ->GetFunction(context)
          .ToLocalChecked();
  // kInternalized strings are created in the old space.
  const v8::NewStringType type = v8::NewStringType::kInternalized;
  v8::Local<v8::String> name_string =
      v8::String::NewFromUtf8(isolate(), name, type).ToLocalChecked();
  function->SetName(name_string);  // NODE_SET_METHOD() compatibility.
}


}

#endif // SRC_ENV_INL_H_