build:
	@g++ -I ./deps/v8/include ./src/*.cc -o ./xnode -lv8 -lv8_libbase -lv8_libplatform -std=c++11

.PHONY: build