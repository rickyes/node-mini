CC         = g++
STD        = c++0x
NODE       = ./xnode
SOURCE     = ./src/*.cc
INCLUDE    = ./deps/v8/include
V8_RELEASE = deps/v8/out.gn/x64.release.sample/obj/

build:
	@g++ -Ideps/v8/ -I$(INCLUDE) $(SOURCE) -o $(NODE) -lv8_monolith -L$(V8_RELEASE) -pthread -std=$(STD)

install:
	@cp xnode /usr/local/bin/xnode

uninstall:
	@rm -rf /usr/local/bin/xnode

test:
	@$(NODE) test/hello.js

.PHONY: build test install uninstall
