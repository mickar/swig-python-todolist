from todolist import *

import threading
import time

def foo(name, counter, tsleep):
    n = 0
    while counter > 0:
        n += 1
        print("Thread name(%s) %d" % (name, n))
        counter-=1
        time.sleep(tsleep)

threads = []
for i in range(2):
    t = threading.Thread(target=foo, args=("display_0"+str(i),10,1))
    threads.append(t)

for t in threads:
    t.start()

time.sleep(2)

t = todolist()
t.addTodo("blabla", "bla")
todolist.UnderstandGILNonBlocked()

# Wait for all threads to complete
for t in threads:
    t.join()


print("Exiting Main Thread")
