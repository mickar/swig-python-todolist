%module todolist
%{
  #include "todolist.h"
%}

%include "std_string.i"

%newobject OPT;
/*
namespace std {
    %template(OpentelemetryLabels) map<string, string>;
}
*/
%catches(todoerror) todolist::addTodo(const char* taskname, const char* tasktodo);
%catches(todoerror) todolist::delTodo(const char* taskname);

%include "todolist.h"

