from todolist import *

t = todolist()
t.addTodo("blabla", "bla")

try:
    t.addTodo("blabla", "bla")
except todoerror as e:
    print("throw --> " + (e.what()))

