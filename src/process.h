#ifndef __PROCESS__H_
#define __PROCESS__H_

#include "v8.h"

class process {
    public:
        static void print(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void print_error(const v8::FunctionCallbackInfo<v8::Value>& args);
        const static char* ToCString(const v8::String::Utf8Value& value);
};

#endif