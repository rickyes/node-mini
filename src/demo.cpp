#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libplatform/libplatform.h"
#include "v8.h"

using namespace v8;

Local<Array> NewPointArray (int x, int y, int z) {
  Isolate* isolate = Isolate::GetCurrent();
  // 临时句柄
  EscapableHandleScope handle_scope(isolate);
  Local<Array> array = Array::New(isolate, 3);
  if (array.IsEmpty()) {
    return Local<Array>();
  }
  array->Set(0, Integer::New(isolate, x));
  array->Set(1, Integer::New(isolate, y));
  array->Set(2, Integer::New(isolate, z));
  return handle_scope.Escape(array);
}

int main(int argc, char* argv[]) {
  V8::InitializeICUDefaultLocation(argv[0]);
  V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<Platform> platform = platform::NewDefaultPlatform();
  V8::InitializePlatform(platform.get());
  V8::Initialize();

  Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = 
    ArrayBuffer::Allocator::NewDefaultAllocator();
  Isolate* isolate = Isolate::New(create_params);
  {
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    Local<Context> context = Context::New(isolate);
    Context::Scope context_scope(context);
    {
        Local<Array> array = NewPointArray(1,2,3);
        for(int i=0 ; i < 3; i++ ) {
          Local<Value> value = array->Get(i);
          uint32_t number = value->Uint32Value(context).ToChecked();
          printf("the number is: %u\n", number);
        }
    }
  }
  return 0;
}