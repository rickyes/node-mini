CC         = g++
STD        = c++14
NODE       = ./xnode
SOURCE     = ./src/*.cc
INCLUDE    = ./deps/v8/include
# INCLUDE_UV = ./deps/libuv/include
V8_RELEASE = deps/v8/out.gn/x64.release.sample/obj/

BUILDTYPE ?= Release
BUILDTYPE_LOWER := $(shell echo $(BUILDTYPE) | tr '[A-Z]' '[a-z]').sample
UNAME_M=$(shell uname -m)

ifeq ($(findstring x86_64,$(UNAME_M)),x86_64)
DESTCPU ?= x64
else
ifeq ($(findstring amd64,$(UNAME_M)),amd64)
DESTCPU ?= x64
else
ifeq ($(findstring ppc64,$(UNAME_M)),ppc64)
DESTCPU ?= ppc64
else
ifeq ($(findstring ppc,$(UNAME_M)),ppc)
DESTCPU ?= ppc
else
ifeq ($(findstring s390x,$(UNAME_M)),s390x)
DESTCPU ?= s390x
else
ifeq ($(findstring s390,$(UNAME_M)),s390)
DESTCPU ?= s390
else
ifeq ($(findstring arm,$(UNAME_M)),arm)
DESTCPU ?= arm
else
ifeq ($(findstring aarch64,$(UNAME_M)),aarch64)
DESTCPU ?= arm64
else
ifeq ($(findstring powerpc,$(shell uname -p)),powerpc)
DESTCPU ?= ppc64
else
DESTCPU ?= x86
endif
endif
endif
endif
endif
endif
endif
endif
endif
ifeq ($(DESTCPU),x64)
ARCH=x64
else
ifeq ($(DESTCPU),arm)
ARCH=arm
else
ifeq ($(DESTCPU),arm64)
ARCH=arm64
else
ifeq ($(DESTCPU),ppc64)
ARCH=ppc64
else
ifeq ($(DESTCPU),ppc)
ARCH=ppc
else
ifeq ($(DESTCPU),s390)
ARCH=s390
else
ifeq ($(DESTCPU),s390x)
ARCH=s390x
else
ARCH=x86
endif
endif
endif
endif
endif
endif
endif

ifeq ($(DESTCPU),x86)
V8_ARCH=ia32
else
V8_ARCH ?= $(DESTCPU)
endif

ifdef DISABLE_V8_I18N
	V8_BUILD_OPTIONS += i18nsupport=off
endif

all: v8 build

build:
	@g++ -Ideps/v8 -I$(INCLUDE) $(SOURCE) -o $(NODE) -lv8_monolith -L$(V8_RELEASE) -pthread -std=$(STD) -DV8_COMPRESS_POINTERS

v8:
	tools/make-v8.sh $(V8_ARCH).$(BUILDTYPE_LOWER) $(V8_BUILD_OPTIONS)

install:
	@cp xnode /usr/local/bin/xnode

uninstall:
	@rm -rf /usr/local/bin/xnode

test:
	@$(NODE) test/hello.js

.PHONY: build test install uninstall
