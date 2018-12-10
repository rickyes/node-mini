#include "include/libplatform/libplatform.h"
#include "include/v8.h"
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream> 
#include <map>
#include "shell.h"
#include "process.h"
#include "env.h"
#include "xnode.h"
#include "xnode_internals.h"
#include <assert.h>

using v8::V8;
using std::map;
using std::string;
using v8::MaybeLocal;
using v8::String;
using v8::Isolate;
using v8::NewStringType;
using v8::Local;
using v8::Value;
using v8::ObjectTemplate;
using v8::Handle;
using v8::FunctionTemplate;
using v8::Context;
using v8::Script;
using v8::TryCatch;
using v8::HandleScope;
using v8::Message;
using v8::Object;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::EscapableHandleScope;
using v8::ScriptOrigin;

namespace xnode {

static xnode_module* modlist_builtin;
static xnode_module* modlist_internal;
static xnode_module* modlist_addon;
static xnode_module* modlist_linked;
static bool node_is_initialized;

void init_v8(const char* argv[]) {
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> plateform = v8::platform::NewDefaultPlatform();
    V8::InitializePlatform(plateform.get());
    V8::Initialize();
}

std::string readFile(const char* filename) {
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    return contents;
}


MaybeLocal<String> ReadFile(Isolate* isolate, const string& name) {
    FILE* file = fopen(name.c_str(), "rb");
    if (file == NULL) return MaybeLocal<String>();

    fseek(file, 0 ,SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    std::unique_ptr<char> chars(new char[size + 1]);
    chars.get()[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars.get()[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return MaybeLocal<String>();
        }
    }
    fclose(file);
    MaybeLocal<String> result = String::NewFromUtf8(
        isolate, chars.get(), NewStringType::kNormal, static_cast<int>(size));
    return result;
}


void parseException(Isolate* isolate, TryCatch* try_catch) {
  HandleScope handle_scope(isolate);
  String::Utf8Value exception(isolate, try_catch->Exception());
  const char* exception_string = process::ToCString(exception);
  Local<Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    fprintf(stderr, "%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    String::Utf8Value filename(isolate,
                                   message->GetScriptOrigin().ResourceName());
    Local<v8::Context> context(isolate->GetCurrentContext());
    const char* filename_string = process::ToCString(filename);
    int linenum = message->GetLineNumber(context).FromJust();
    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    String::Utf8Value sourceline(
        isolate, message->GetSourceLine(context).ToLocalChecked());
    const char* sourceline_string = process::ToCString(sourceline);
    fprintf(stderr, "%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn(context).FromJust();
    for (int i = 0; i < start; i++) {
      fprintf(stderr, " ");
    }
    int end = message->GetEndColumn(context).FromJust();
    for (int i = start; i < end; i++) {
      fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
    Local<v8::Value> stack_trace_string;
    if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
        stack_trace_string->IsString() &&
        Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
      String::Utf8Value stack_trace(isolate, stack_trace_string);
      const char* stack_trace_string = process::ToCString(stack_trace);
      fprintf(stderr, "%s\n", stack_trace_string);
    }
  }
}

void compile(Local<String> source, Isolate* isolate, Local<v8::Context> context) {
    TryCatch try_catch(isolate);
    Local<Script> script;
    v8::ScriptOrigin origin(String::NewFromUtf8(isolate, "(xnode)"));
    if (!Script::Compile(context, source, &origin).ToLocal(&script)) {
      parseException(isolate, &try_catch);
      return;
    }
    Local<Value> result;
    if (!script->Run(context).ToLocal(&result)) {
        assert(try_catch.HasCaught());
        parseException(isolate, &try_catch);
        return;
    }
}



void ParseOptions(int argc,
                  char* argv[],
                  map<string, string>* options,
                  string* file) {
    for(int i=1; i < argc; i++) {
        string arg = argv[i];
        size_t index = arg.find('=', 0);
        if (index == string::npos) {
            *file = arg;
        } else {
            string key = arg.substr(0, index);
            string value = arg.substr(index + 1);
            (*options)[key] = value;
        }
    }
}

inline struct xnode_module* FindModule(struct xnode_module* list,
                                       const char* name,
                                       int flag) {
    struct xnode_module* mp;

    for(mp = list; mp != nullptr; mp = mp->nm_link) {
        if (strcmp(mp->nm_modname, name) == 0) break;
    }
    CHECK(mp == nullptr || (mp->nm_flags & flag) != 0);
    return mp;
}

xnode_module* get_builtin_module(const char* name) {
    return FindModule(modlist_builtin, name, NM_F_BUILTIN);
}

static Local<Object> InitModule(Isolate* isolate,
                                Local<Context> context,
                                xnode_module* mod,
                                Local<String> module) {
    Local<Object> exports = Object::New(isolate);
    
    CHECK_NULL(mod->nm_register);
    CHECK_NO_NULL(mod->nm_context_register_func);
    Local<Value> unused = v8::Undefined(isolate);
    mod->nm_context_register_func(exports,
                                 unused,
                                 context,
                                 mod->nm_priv);
    return exports;
}

static void GetBinding(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    CHECK(args[0]->IsString());

    Local<String> module = args[0].As<String>();
    String::Utf8Value module_v(isolate, module);

    xnode_module* mod = get_builtin_module(*module_v);
    Local<Object> exports;
    if (mod != nullptr) {
        std::cout<< std::string(*module_v) <<std::endl;
        exports = InitModule(isolate, context, mod, module);
    }
    if (!strcmp(*module_v, "natives")) {
        exports = Object::New(isolate);
    }

    args.GetReturnValue().Set(exports);
}

static MaybeLocal<Value> ExecuteString(
    Isolate* isolate,
    Local<Context> context,
    Local<String> source,
    Local<String> filename) {
    EscapableHandleScope scope(isolate);
    TryCatch try_catch(isolate);

    try_catch.SetVerbose(false);

    ScriptOrigin origin(filename);
    MaybeLocal<Script> script = 
        v8::Script::Compile(context, source, &origin);
    if (script.IsEmpty()) {
        parseException(isolate, &try_catch);
        return MaybeLocal<Value>();
    }

    MaybeLocal<Value> result = script.ToLocalChecked()->Run(context);
    if (result.IsEmpty()) {
        if (try_catch.HasTerminated()) {
            isolate->CancelTerminateExecution();
            return MaybeLocal<Value>();
        }
        parseException(isolate, &try_catch);
        return MaybeLocal<Value>();
    }

    return scope.Escape(result.ToLocalChecked());
}

static MaybeLocal<Function> GetBootstrapper(
    Isolate* isolate,
    Local<Context> context,
    Local<String> source,
    Local<String> script_name) {
  EscapableHandleScope scope(isolate);

  TryCatch try_catch(isolate);

  try_catch.SetVerbose(false);

  MaybeLocal<Value> bootstrapper_v = ExecuteString(isolate, context, source, script_name);
  if (bootstrapper_v.IsEmpty()) {
      return MaybeLocal<Function>();
  }

  if (try_catch.HasCaught()) {
      parseException(isolate, &try_catch);
      exit(10);
  }

  CHECK(bootstrapper_v.ToLocalChecked()->IsFunction());
  return scope.Escape(bootstrapper_v.ToLocalChecked().As<Function>());
}

static bool ExecuteBootstrapper(Isolate* isolate, Local<Context> context, Local<Function> bootstrapper,
                                    int argc, Local<Value> argv[], Local<Value>* out) {
    bool ret = bootstrapper->Call(context, Null(isolate), argc, argv).ToLocal(out);
    return ret;
}

extern "C" void xnode_module_register(void* m) {
    printf("------------");
    struct xnode_module* mp = reinterpret_cast<struct xnode_module*>(m);

    if (mp->nm_flags & NM_F_BUILTIN) {
        mp->nm_link = modlist_builtin;
        modlist_builtin = mp;
    } else if (mp->nm_flags & NM_F_INTERNAL) {
        mp->nm_link = modlist_internal;
        modlist_internal = mp;
    } else if (!node_is_initialized) {
        mp->nm_flags = NM_F_LINKED;
        mp->nm_link = modlist_linked;
        modlist_linked = mp;
    } else {
        //
    }
}

}

int main(int argc, char *argv[]) {
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> plateform = v8::platform::NewDefaultPlatform();
    V8::InitializePlatform(plateform.get());
    V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    if (argc > 1) {
        HandleScope handle_scope(isolate);

        Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);
        global->Set(String::NewFromUtf8(isolate, "print"), FunctionTemplate::New(isolate, xnode::process::print));
        global->Set(String::NewFromUtf8(isolate, "print_error"), FunctionTemplate::New(isolate,xnode::process::print_error));
        Local<Context> context = Context::New(isolate, NULL, global);
        Context::Scope context_scope(context);

        const char* bootstrapJsCore = "lib/console.js";
        Local<String> sourceJsCode;
        if (!xnode::ReadFile(isolate, bootstrapJsCore).ToLocal(&sourceJsCode)) {
            fprintf(stderr, "The startup script was not found.\n");
        }
        xnode::compile(sourceJsCode, isolate, context);

        Local<String> loaders_source;
        const char* loaders_name = "lib/bootstrap/loaders.js";
        if (!xnode::ReadFile(isolate, loaders_name).ToLocal(&loaders_source)) {
            fprintf(stderr, "The startup script was not found.\n");
        }

        MaybeLocal<Function> loaders_bootstrapper = 
            xnode::GetBootstrapper(isolate, context, loaders_source, String::NewFromUtf8(isolate, loaders_name));
        if (loaders_bootstrapper.IsEmpty()) {
            // Execution was interrupted.
            return 0;
        }

        Local<Function> get_binding_fn = Function::New(isolate, xnode::GetBinding);
        Local<Value> loaders_bootstrapper_args[] = {
            Object::New(isolate),
            get_binding_fn
        };
        Local<Value> bootstrapped_loaders;
        if (!xnode::ExecuteBootstrapper(isolate, context, loaders_bootstrapper.ToLocalChecked(),
                                2, loaders_bootstrapper_args, &bootstrapped_loaders)) {
            return 0;
        }

        const char* filename = argv[1];
        Local<String> source;
        if (!xnode::ReadFile(isolate, filename).ToLocal(&source)) {
           fprintf(stderr, "No script was specified.\n");
        }

        xnode::compile(source, isolate, context);
    } else {
        return xnode::shell::init(argc, argv);
    }

    return 0;
}