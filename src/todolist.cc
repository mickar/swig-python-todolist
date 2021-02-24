#include <stdio.h>
#include "todolist.h"

todolist::todolist(){
	printf("Constructor\n");
}

todolist::~todolist(){
	printf("Destructor\n");
}

int todolist::addTodo(const char* taskname, const char* tasktodo) {
	std::string s1(taskname), s2(tasktodo);
	std::map<std::string, std::string>::iterator it = this->todolist_.find(s1);
	if (it != this->todolist_.end()) {
		printf("ERROR: already exist\n");
		return -1;
	}
	this->todolist_.insert(std::pair<std::string,std::string>(s1, s2));
	return 0;
}

int todolist::delTodo(const char* taskname){
	std::string s1(taskname), s2;
	std::map<std::string, std::string>::iterator it = this->todolist_.find(s1);
	if (it == this->todolist_.end()) {
		printf("ERROR: not found\n");
		return -1;
	}
	s2 = it->second;
	this->todolist_.erase(it);
	printf("%s deleted\n", s2.c_str());
	return 0;
}

void todolist::displayTodoList() {

}

const char* todolist::version() {
	return "Version TodoList0.1.0\n";
}

