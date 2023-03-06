ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC := src

PROJECT_NAME=todolist
PYTHON_VERSION=3
PYTHON=/usr/bin/python${PYTHON_VERSION}
FILE?=
CXXFLAGS= -c -fPIC  `python${PYTHON_VERSION}-config --cflags`
CXXFLAGS_OPT= -I/usr/local/include/ -L/usr/local/lib/ -lopentelemetry_resources -lopentelemetry_common -lopentelemetry_metrics -lopentelemetry_exporter_prometheus -lprometheus-cpp-pull -lprometheus-cpp-core -lpthread -lm -lz -lc
DEBUGFLAGS=

all:
	g++ ${DEBUGFLAGS} ${CXXFLAGS} ${SRC}/todolist.cc -o ${SRC}/todolist.o
	g++ ${DEBUGFLAGS} -rdynamic ${CXXFLAGS} ${CXXFLAGS_OPT} ${SRC}/opt.cc -o ${SRC}/opt.o
	swig -c++ -python -o ${SRC}/todolist_wrap.cxx ${SRC}/todolist.i
	g++ ${DEBUGFLAGS} ${CXXFLAGS} -I${SRC}/ ${SRC}/todolist_wrap.cxx -o ${SRC}/todolist_wrap.o
	g++ ${DEBUGFLAGS} -Xlinker -export-dynamic -shared ${SRC}/todolist.o ${SRC}/todolist_wrap.o ${SRC}/opt.o -o ${SRC}/_todolist.so

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
	docker run -it --name python-$(PROJECT_NAME) --rm -v `pwd`:/usr/local/src/swig-python-todolist debian:11 bash /usr/local/src/swig-python-todolist/build/devinit.sh

docker-attach:
	docker exec -it $(PYTHON)-$(PROJECT_NAME) bash

.PHONY: clean
