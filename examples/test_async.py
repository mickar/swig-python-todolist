from todolist import *
import asyncio
import uvloop

t1 = todolist("MainTask")

async def task01(s):
	t2 = todolist("Task01")
	t2.addTodo("blabla1", s)
	t1.addTodo("blabla1", s)
	t2.dumpAllTodo()

async def task02(s):
	t1.addTodo("blabla2", s)
	t1.dumpAllTodo()

uvloop.install()
asyncio.run(task01("blaA"))
asyncio.run(task02("bla2"))
