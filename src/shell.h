#ifndef __SHELL_H_
#define __SHELL_H_

#include <v8.h>
#include <libplatform/libplatform.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class shell {
    public:
       static int init(int argc, char* argv[]);
};

#endif