#ifndef SRC_XNODE_BINDING_H__
#define SRC_XNODE_BINDING_H__
#include "xnode.h"
#include <assert.h>

enum {
  NM_F_BUILTIN  = 1 << 0,
  NM_F_LINKED   = 1 << 1,
  NM_F_INTERNAL = 1 << 2,
};

static_assert(static_cast<int>(NM_F_LINKED) ==
              static_cast<int>(xnode::ModuleFlags::kLinked),
              "NM_F_LINKED != node::ModuleFlags::kLinked");

#define XNODE_MODULE_CONTEXT_AWARE_CPP(modname, regfunc, priv, flags)           \
  static xnode::xnode_module _module = {                                         \
      XNODE_MODULE_VERSION,                                                     \
      flags,                                                                   \
      nullptr,                                                                 \
      __FILE__,                                                                \
      nullptr,                                                                 \
      (xnode::addon_context_register_func)(regfunc),                            \
      NODE_STRINGIFY(modname),                                                 \
      priv,                                                                    \
      nullptr};                                                                \
  void _register_##modname() { xnode_module_register(&_module); }

namespace xnode {
    #define XNODE_BUILTIN_MODULE_CONTEXT_AWARE(modname, regfunc)          \
      XNODE_MODULE_CONTEXT_AWARE_CPP(modname, regfunc, nullptr, NM_F_BUILTIN)

    namespace binding {
      void RegisterBuiltinModules();
    }
}

#endif