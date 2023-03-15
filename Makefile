ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC := src

PROJECT_NAME=todolist
PYTHON_VERSION=3
PYTHON=/usr/bin/python${PYTHON_VERSION}
FILE?=
CXXFLAGS= `python${PYTHON_VERSION}-config --cflags`
CXXFLAGS_OPT= -I/usr/local/include/ -L/usr/local/lib/ -lopentelemetry_resources -lopentelemetry_common -lopentelemetry_metrics -lopentelemetry_exporter_prometheus -lprometheus-cpp-pull -lprometheus-cpp-core -lpthread -lm -lz -lc  -DENABLE_LOGS_PREVIEW -lopentelemetry_exporter_ostream_span -lopentelemetry_exporter_ostream_logs -lopentelemetry_trace -lopentelemetry_logs -lopentelemetry_exporter_otlp_http -lopentelemetry_exporter_otlp_http_log
DEBUGFLAGS= 

all:
	g++ -std=c++17 -c -fPIC ${DEBUGFLAGS} ${SRC}/todolist.cc -o ${SRC}/todolist.o ${CXXFLAGS} ${CXXFLAGS_OPT}
	swig -c++ -python -o ${SRC}/todolist_wrap.cxx ${SRC}/todolist.i
	g++ -std=c++17 -c -fPIC ${DEBUGFLAGS} -I${SRC}/ ${SRC}/todolist_wrap.cxx -o ${SRC}/todolist_wrap.o ${CXXFLAGS} ${CXXFLAGS_OPT}
	g++ -std=c++17 -c -fPIC ${DEBUGFLAGS} ${SRC}/opt.cc -o ${SRC}/opt.o ${CXXFLAGS} ${CXXFLAGS_OPT}
	g++ -std=c++17 -shared ${DEBUGFLAGS} ${SRC}/todolist.o ${SRC}/todolist_wrap.o ${SRC}/opt.o -o ${SRC}/_todolist.so ${CXXFLAGS} ${CXXFLAGS_OPT} ${DEBUGFLAGS}
	g++ -std=c++17 -c -fPIC ${DEBUGFLAGS} ${SRC}/opt_test.cc -o ${SRC}/opt_test.o ${CXXFLAGS} ${CXXFLAGS_OPT}
	g++ -std=c++17 ${DEBUGFLAGS} ${SRC}/opt_test.o ${SRC}/opt.o -o ${SRC}/opt_test ${CXXFLAGS} ${CXXFLAGS_OPT} ${DEBUGFLAGS}

clean:
	rm -f ${SRC}/*.o ${SRC}/*.so ${SRC}/todolist_wrap.* ${SRC}/todolist.py*

debug: DEBUGFLAGS += -DDEBUG -g -lefence
debug: all

tests:
	@for filename in $$(find ./examples -type f -name "*.py"); do \
		echo "------------------------------------------------------\n"; \
		echo -n "\nTEST $${filename} - Command: "; \
		echo 'PYTHONPATH=$(ROOT_DIR)/src $(PYTHON) $${filename}\n'; \
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
	docker run -it --name python-$(PROJECT_NAME) --rm -v `pwd`:/usr/local/src/swig-python-todolist debian:10 bash /usr/local/src/swig-python-todolist/build/devinit.sh

docker-attach:
	docker exec -it $(PYTHON)-$(PROJECT_NAME) bash

.PHONY: clean
