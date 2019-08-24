#ifndef __ENV_H__
#define __ENV_H__

#include "v8.h"

namespace xnode {

#define ENVIRONMENT_STRONG_PERSISTENT_VALUES(V) \
    V(as_callback_data_template, v8::FunctionTemplate)   \
    V(as_callback_data, v8::Object)

#define V(PropertyName, TypeName) v8::Global<TypeName> PropertyName ## _;
  ENVIRONMENT_STRONG_PERSISTENT_VALUES(V)
#undef V

#define V(PropertyName, TypeName)                                             \
  inline v8::Local<TypeName> PropertyName() const;                            \
  inline void set_ ## PropertyName(v8::Local<TypeName> value);                \
  ENVIRONMENT_STRONG_PERSISTENT_VALUES(V)
#undef V

class Environment {
    public:
        Environment(v8::Local<v8::Context> context);
        ~Environment();
        static inline Environment* GetFromCallbackData(v8::Local<v8::Value> val);
        static inline Environment* GetCurrent(v8::Isolate* isolate);
        static inline Environment* GetCurrent(v8::Local<v8::Context> context);
        static inline Environment* GetCurrent(
            const v8::FunctionCallbackInfo<v8::Value>& info);
        template <typename T>
        static inline Environment* GetCurrent(
            const v8::PropertyCallbackInfo<T>& info);
        static inline Environment* getCurrent(
            const v8::FunctionCallbackInfo<v8::Value>& args);

        inline v8::Isolate* isolate() const;
        inline v8::Local<v8::Context> context() const;
        inline v8::Local<v8::FunctionTemplate>
      NewFunctionTemplate(v8::FunctionCallback callback,
                          v8::Local<v8::Signature> signature =
                              v8::Local<v8::Signature>(),
                          v8::ConstructorBehavior behavior =
                              v8::ConstructorBehavior::kAllow,
                          v8::SideEffectType side_effect =
                              v8::SideEffectType::kHasSideEffect);
        inline void SetMethod(v8::Local<v8::Object> that,
                        const char* name,
                        v8::FunctionCallback callback); 
    private:
        v8::Isolate* const isolate_;
        void CreateProperties();
};

} // xnode 

#endif