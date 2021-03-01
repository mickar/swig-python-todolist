#ifndef TODOLIST_H
#define TODOLIST_H

#include <map>

class todoerror {
	private:
		char msg_[512];
	public:
		todoerror(const char *msg);
		const char* what();
};

class todolist {
	private:
		std::map<std::string, std::string> todolist_;
	public:
		todolist();
		~todolist();

		void addTodo(const char* taskname, const char* tasktodo);  
		void delTodo(const char* taskname);
		void displayTodoList();
		static const char* version();
};

#endif
