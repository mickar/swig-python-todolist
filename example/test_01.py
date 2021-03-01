import  todolist
t = todolist.todolist()
try:
    t.addTodo("blabla", "bla")
except todolist.todoerror as e:
    print("throw --> " + (e.what()))

try:
    t.addTodo("blabla", "bla")
except todolist.todoerror as e:
    print("throw --> " + (e.what()))

