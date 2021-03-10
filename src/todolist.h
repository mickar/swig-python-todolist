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
		std::string name;
		std::map<std::string, std::string> todolist_;
	public:
		todolist(const char *name = NULL);
		~todolist();

		void addTodo(const char* taskname, const char* tasktodo);  
		void delTodo(const char* taskname);
		void dumpAllTodo();
		static const char* version();
		static void UnderstandGILBlocked();
		static void UnderstandGILNonBlocked();
};

#endif
