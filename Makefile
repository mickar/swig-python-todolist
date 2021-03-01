ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC := src

all:
	g++ -c -fPIC `python-config --cflags` ${SRC}/todolist.cc -o ${SRC}/todolist.o
	swig -c++ -python -o ${SRC}/todolist_wrap.cxx ${SRC}/todolist.i
	g++ -c -fPIC `python-config --cflags` -I${SRC}/ ${SRC}/todolist_wrap.cxx -o ${SRC}/todolist_wrap.o
	g++ -Xlinker -export-dynamic -shared ${SRC}/todolist.o ${SRC}/todolist_wrap.o -o ${SRC}/_todolist.so

clean:
	rm -f ${SRC}/*.o ${SRC}/*.so ${SRC}/todolist_wrap.* ${SRC}/todolist.py*

test:
	echo "TEST 1:"
	PYTHONPATH=$(ROOT_DIR)/src python example/test_01.py
	echo "TEST 2:"
	PYTHONPATH=$(ROOT_DIR)/src python example/test_02.py

.PHONY: clean
