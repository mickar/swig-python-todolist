#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Python.h>

#include "todolist.h"
#include "opt.h"


todoerror::todoerror(const char *msg) {
	strncpy(this->msg_, msg, 512);
}

const char* todoerror::what() {
	return this->msg_;
}

todolist::todolist(const char *str){
	if (str != NULL) {
		this->name = std::string(str);
	}
	auto metrics = new Metrics("0.0.0.0:8081", "opt_todolist");
	metrics->CreateMetricGauge("nbtodolist", "up and down meter");
	printf("Constructor %s\n", this->name.c_str());
}

todolist::~todolist(){
	printf("Destructor %s\n", this->name.c_str());
}

void todolist::addTodo(const std::string &taskname, const std::string &tasktodo) {
	std::string s1(taskname), s2(tasktodo);
	std::map<std::string, std::string>::iterator it = this->todolist_.find(s1);
	if (it != this->todolist_.end()) {
		throw todoerror("Alreay exist");
	}
	Metrics::UpdateMeterGaugeAdd("nbtodolist", 1);
	this->todolist_.insert(std::pair<std::string,std::string>(s1, s2));
}

void todolist::delTodo(const std::string &taskname){
	std::string s1(taskname), s2;
	std::map<std::string, std::string>::iterator it = this->todolist_.find(s1);
	if (it == this->todolist_.end()) {
		throw todoerror("Not Found");
	}
	s2 = it->second;
	this->todolist_.erase(it);
	Metrics::UpdateMeterGaugeAdd("nbtodolist", -1);
	printf("%s deleted\n", s2.c_str());
}

void todolist::dumpAllTodo() {
	printf("Dump All TODO [%s] :\n", this->name.c_str());
	for (auto it = this->todolist_.begin(); it != this->todolist_.end(); ++it) {
		printf("\t%s - %s\n", it->second.c_str(), it->first.c_str());
	}
}

const char* todolist::version() {
	return "Version TodoList0.1.0\n";
}

void todolist::UnderstandGILBlocked() {
	printf("Begin\n");
	sleep(10);
	printf("End\n");
}

void todolist::UnderstandGILNonBlocked() {
	printf("Begin\n");
	Py_BEGIN_ALLOW_THREADS
	sleep(10);
	Py_END_ALLOW_THREADS
	printf("End\n");
}

