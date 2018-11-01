#ifndef __MODULE___H_
#define __MODULE___H_

#include "v8.h"

class module {
    public:
        v8::String require(v8::String path);
        
};

#endif