ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC := src

CXXFLAGS= -c -fPIC `python3-config --cflags`
DEBUGFLAGS=

all:
	g++ ${DEBUGFLAGS} ${CXXFLAGS} ${SRC}/todolist.cc -o ${SRC}/todolist.o
	swig -c++ -python -o ${SRC}/todolist_wrap.cxx ${SRC}/todolist.i
	g++ ${DEBUGFLAGS} ${CXXFLAGS} -I${SRC}/ ${SRC}/todolist_wrap.cxx -o ${SRC}/todolist_wrap.o
	g++ ${DEBUGFLAGS} -Xlinker -export-dynamic -shared ${SRC}/todolist.o ${SRC}/todolist_wrap.o -o ${SRC}/_todolist.so

clean:
	rm -f ${SRC}/*.o ${SRC}/*.so ${SRC}/todolist_wrap.* ${SRC}/todolist.py*

debug: DEBUGFLAGS += -DDEBUG -g -lefence
debug: all

tests:
	@for file in $$(find ./examples -type f -name "*.py"); do \
		echo "------------------------------------------------------\n"; \
		echo "\nTEST $${file} \n"; \
		PYTHONPATH=$(ROOT_DIR)/src python3 $${file}; \
		echo "\n"; \
	done


docker-run:
	docker run -it --rm -v `pwd`:/usr/local/src/swig-python-todolist debian bash /usr/local/src/swig-python-todolist/build/devinit.sh

.PHONY: clean
