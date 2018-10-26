CC      = g++
STD     = c++0x
NODE    = ./xnode
SOURCE  = ./src/*.cc
INCLUDE = ./deps/v8/include

build:
	@$(CC) -I $(INCLUDE) $(SOURCE) -o $(NODE) -lv8 -lv8_libbase -lv8_libplatform -std=$(STD)

install:
	@cp xnode /usr/local/bin/xnode

uninstall:
	@rm -rf /usr/local/bin/xnode

test:
	@$(NODE) test/hello.js

.PHONY: build test install uninstall
