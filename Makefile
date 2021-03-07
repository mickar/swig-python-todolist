ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC := src

PROJECT_NAME=todolist
PYTHON_VERSION=3
PYTHON=/usr/bin/python${PYTHON_VERSION}
FILE?=
CXXFLAGS= -c -fPIC `python${PYTHON_VERSION}-config --cflags`
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
	@for filename in $$(find ./examples -type f -name "*.py"); do \
		echo "------------------------------------------------------\n"; \
		echo "\nTEST $${filename} \n"; \
		PYTHONPATH=$(ROOT_DIR)/src $(PYTHON) $${filename}; \
		echo "\n"; \
	done

test:
	PYTHONPATH=$(ROOT_DIR)/src $(PYTHON) $(FILE)

gdb:
	PYTHONPATH=$(ROOT_DIR)/src gdb $(PYTHON) $(FILE)

dis:
	$(PYTHON) -m dis $(FILE)

docker-run:
	docker run -it --name $(PYTHON)-$(PROJECT_NAME) --rm -v `pwd`:/usr/local/src/swig-python-todolist debian bash /usr/local/src/swig-python-todolist/build/devinit.sh

docker-attach:
	docker exec -it $(PYTHON)-$(PROJECT_NAME) bash

.PHONY: clean
