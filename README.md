# Sample of SWIG python project
The purpose of this project is to create a sample of python swig use.

The idea is to create a simple repository. This should help to understand Swig tool and make it easy to use.

## Dependencies

### Environment

* [Docker](https://www.docker.com/)
 
### Programming Language

* [Python3](https://www.python.org/download/releases/3.0/)
* [C++](https://isocpp.org/)
* [Swig](http://www.swig.org/Doc1.3/Python.html)

### Debug

* [Electric Fence](https://elinux.org/Electric_Fence)
* [GDB](https://www.gnu.org/software/gdb/)

## Getting started
This project provides a `Makefile` with short commands to run common tasks.

```bash
make docker-run
```
Run this for run a fast and simple docker environment.

```bash
make debug
```
This command will build in debug mode the sample.

```bash
make
```
Use this command to build without debug.

## Running the tests
```bash
make tests
```
Use this command to run unit tests.

