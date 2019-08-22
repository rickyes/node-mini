#ifndef __XNODE_H__
#define __XNODE_H__

#ifdef _WIN32
# ifndef BUILDING_NODE_EXTENSION
#  define NODE_EXTERN __declspec(dllexport)
# else
#  define NODE_EXTERN __declspec(dllimport)
# endif
#else
# define NODE_EXTERN __attribute__((visibility("default")))
#endif

#ifdef BUILDING_NODE_EXTENSION
# undef BUILDING_V8_SHARED
# undef BUILDING_UV_SHARED
# define USING_V8_SHARED 1
# define USING_UV_SHARED 1
#endif

#if defined(__MINGW32__) || defined(_MSC_VER)
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0600  // Windows Server 2008
#endif

#ifndef NOMINMAX
# define NOMINMAX
#endif

#endif

#if defined(_MSC_VER)
#define PATH_MAX MAX_PATH
#endif

#ifdef _WIN32
# define SIGKILL 9
#endif

#include "v8.h"

#define CHECK(pass) do { assert(pass); } while (0)
#define CHECK_NULL(val) CHECK((val) == nullptr)
#define CHECK_NO_NULL(val) CHECK((val) != nullptr)

#define XNODE_MODULE_VERSION 1

#ifndef NODE_STRINGIFY
#define NODE_STRINGIFY(n) NODE_STRINGIFY_HELPER(n)
#define NODE_STRINGIFY_HELPER(n) #n
#endif

namespace xnode {

enum ModuleFlags {
  kLinked = 0x02
};

typedef void (*addon_register_func)(
    v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    void* priv);

typedef void (*addon_context_register_func)(
    v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    void* priv);

struct xnode_module {
    int nm_version;
    unsigned int nm_flags;
    void* nm_dso_handle;
    const char* nm_filename;
    xnode::addon_register_func nm_register;
    xnode::addon_context_register_func nm_context_register_func;
    const char* nm_modname;
    void* nm_priv;
    struct xnode_module* nm_link;
};

extern "C" NODE_EXTERN void xnode_module_register(void* mod);

#define NODE_SET_PROTOTYPE_METHOD NODE_SET_PROTOTYPE_METHOD

inline void NODE_SET_PROTOTYPE_METHOD(v8::Local<v8::FunctionTemplate> recv,
                                      const char* name,
                                      v8::FunctionCallback callback) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Signature> s = v8::Signature::New(isolate, recv);
    v8::Local<v8::FunctionTemplate> t = 
             v8::FunctionTemplate::New(isolate, callback, v8::Local<v8::Value>(), s);
    v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name,
             v8::NewStringType::kInternalized).ToLocalChecked();
    t->SetClassName(fn_name);
    recv->PrototypeTemplate()->Set(fn_name, t);     
}

}

#endif