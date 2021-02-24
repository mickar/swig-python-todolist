SRC := src

all:
	g++ -c -fPIC `python-config --cflags` ${SRC}/todolist.cc -o ${SRC}/todolist.o
	swig -c++ -python -o ${SRC}/todolist_wrap.cxx ${SRC}/todolist.i
	g++ -c -fPIC `python-config --cflags` -I${SRC}/ ${SRC}/todolist_wrap.cxx -o ${SRC}/todolist_wrap.o
	g++ -Xlinker -export-dynamic -shared ${SRC}/todolist.o ${SRC}/todolist_wrap.o -o ${SRC}/_todolist.so

clean:
	rm -f ${SRC}/*.o ${SRC}/*.so ${SRC}/todolist_wrap.* ${SRC}/todolist.py*

.PHONY: clean
