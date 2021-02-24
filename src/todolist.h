#ifndef TODOLIST_H
#define TODOLIST_H

#include <map>

class todolist {
	private:
		std::map<std::string, std::string> todolist_;
	public:
		todolist();
		~todolist();

		int addTodo(const char* taskname, const char* tasktodo);  
		int delTodo(const char* taskname);
		void displayTodoList();
		static const char* version();
};

#endif
