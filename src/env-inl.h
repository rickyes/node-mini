
#ifndef SRC_ENV_INL_H_
#define SRC_ENV_INL_H_

#include "env.h"
#include "v8.h"
#include "util.h"
#include "xnode_context_data.h"

namespace xnode {

inline Environment* Environment::GetFromCallbackData(v8::Local<v8::Value> val) {
    DCHECK(val->IsObject());
    v8::Local<v8::Object> obj = val.As<v8::Object>();
    // 获取此对象的内部字段数
    DCHECK_GE(obj->InternalFieldCount(), 1);
    // GetAlignedPointerFromInternalField c++ 函数钩子
    Environment* env =
        static_cast<Environment*>(obj->GetAlignedPointerFromInternalField(0));
    // HasInstance 如果给定对象是此函数模板的实例，则返回true
    DCHECK(env->as_callback_data_template()->HasInstance(obj));
    return env;
}

inline Environment* Environment::GetCurrent(v8::Isolate* isolate) {
    // isolate 如果有context则返回true
    if (UNLIKELY(!isolate->InContext())) return nullptr;
    // scope 可以看成是句柄的容器, scope（作用域） 中的所有 handle（对象、引用）就都会被统一释放掉了
    v8::HandleScope handle_scope(isolate);
    return GetCurrent(isolate->GetCurrentContext());
}

// 获取当前上下文
inline Environment* Environment::GetCurrent(v8::Local<v8::Context> context) {
    if (UNLIKELY(context.IsEmpty())) return nullptr;
    if (UNLIKELY(context->GetNumberOfEmbedderDataFields() <= 
                ContextEmbedderIndex::kContextTag)) return nullptr;
    // if (UNLIKELY(context->GetAlignedPointerFromEmbedderData(ContextEmbedderIndex::kContextTag !=
    //             Environment::kN))) 
    
    return static_cast<Environment*>(
        context->GetAlignedPointerFromEmbedderData(
            ContextEmbedderIndex::kEnvironment));
}

inline Environment* Environment::GetCurrent(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
    return GetFromCallbackData(info.Data());
}

template <typename T>
inline Environment* Environment::GetCurrent(
    const v8::PropertyCallbackInfo<T>& info) {
    return GetFromCallbackData(info.Data());
}

inline v8::Isolate* Environment::isolate() const {
  return isolate_;
}

inline v8::Local<v8::FunctionTemplate>
    Environment::NewFunctionTemplate(v8::FunctionCallback callback,
                                     v8::Local<v8::Signature> signature,
                                     v8::ConstructorBehavior behavior,
                                     v8::SideEffectType side_effect_type) {
  v8::Local<v8::Object> external = as_callback_data();
  return v8::FunctionTemplate::New(isolate(), callback, external,
                                   signature, 0, behavior, side_effect_type);
}

// 添加方法
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