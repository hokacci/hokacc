.PHONY: build test clean

PROJECT_NAME = hokacc

build:
	mkdir -p build && cd build && cmake .. &&  $(MAKE)


test:
	test/test.py


clean:
	rm -r build tmp tmp.s

