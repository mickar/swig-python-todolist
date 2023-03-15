%module todolist
%{
  #include "todolist.h"
%}

%include "std_string.i"
%inline %{
using namespace std;
%}

%include "std_map.i"

%newobject Metrics;
%newobject Logs;
%newobject Traces;

namespace std {
    %template(OpentelemetryLabels) map<string, string>;
}

%catches(todoerror) todolist::addTodo(const char* taskname, const char* tasktodo);
%catches(todoerror) todolist::delTodo(const char* taskname);

/** insert the following includes into generated code so it compiles */
%{
#include "opt.h"
%}

%include "opt.h"
%include "todolist.h"

