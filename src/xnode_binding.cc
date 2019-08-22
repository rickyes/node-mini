
#include "xnode_binding.h"

#if HAVE_OPENSSL
#define NODE_BUILTIN_OPENSSL_MODULES(V) V(crypto) V(tls_wrap)
#else
#define NODE_BUILTIN_OPENSSL_MODULES(V)
#endif

#if NODE_HAVE_I18N_SUPPORT
#define NODE_BUILTIN_ICU_MODULES(V) V(icu)
#else
#define NODE_BUILTIN_ICU_MODULES(V)
#endif

#if NODE_REPORT
#define NODE_BUILTIN_REPORT_MODULES(V) V(report)
#else
#define NODE_BUILTIN_REPORT_MODULES(V)
#endif

#if HAVE_INSPECTOR
#define NODE_BUILTIN_PROFILER_MODULES(V) V(profiler)
#else
#define NODE_BUILTIN_PROFILER_MODULES(V)
#endif

#if HAVE_DTRACE || HAVE_ETW
#define NODE_BUILTIN_DTRACE_MODULES(V) V(dtrace)
#else
#define NODE_BUILTIN_DTRACE_MODULES(V)
#endif

#define NODE_BUILTIN_STANDARD_MODULES(V)                                       \
  V(contextify)                                                                

#define NODE_BUILTIN_MODULES(V)                                                \
  NODE_BUILTIN_STANDARD_MODULES(V)                                             \
  NODE_BUILTIN_OPENSSL_MODULES(V)                                              \
  NODE_BUILTIN_ICU_MODULES(V)                                                  \
  NODE_BUILTIN_REPORT_MODULES(V)                                               \
  NODE_BUILTIN_PROFILER_MODULES(V)                                             \
  NODE_BUILTIN_DTRACE_MODULES(V)


#define V(modname) void _register_##modname();
NODE_BUILTIN_MODULES(V)
#undef V

namespace xnode {

namespace binding {
// Call built-in modules' _register_<module name> function to
// do module registration explicitly.
void RegisterBuiltinModules() {
#define V(modname) _register_##modname();
  NODE_BUILTIN_MODULES(V)
#undef V
}

}

}
