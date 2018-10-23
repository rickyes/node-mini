#include "libplatform/libplatform.h"
#include "v8.h"
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream> 
#include <map>
#include "shell.h"

using v8::V8;
using std::map;
using std::string;
using v8::MaybeLocal;
using v8::String;
using v8::Isolate;
using v8::NewStringType;
using v8::Local;
using v8::Value;

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

void compile(string source, Isolate* isolate) {
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> contents = v8::String::NewFromUtf8(isolate, source.c_str(), v8::NewStringType::kNormal)
        .ToLocalChecked();
    v8::Local<v8::Script> script = 
        v8::Script::Compile(context, contents).ToLocalChecked();
    v8::Local<v8::Value> result = script -> Run(context).ToLocalChecked();

    uint32_t number = result->Uint32Value(context).ToChecked();
    printf("3 + 4 = %u\n", number);
}

void compile(Local<String> source, Isolate* isolate) {
    Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);
    Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
    Local<Value> result = script->Run(context).ToLocalChecked();
    uint32_t number = result->Uint32Value(context).ToChecked();
    printf("3 + 4 = %u\n", number);
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
        const char* filename = argv[1];
        Local<String> source;
        if (!ReadFile(isolate, filename).ToLocal(&source)) {
           fprintf(stderr, "No script was specified.\n");
        }
        compile(source, isolate);
    } else {
        return shell::init(argc, argv);
    }

    return 0;
}
