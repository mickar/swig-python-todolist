from todolist import *
import time

Traces.Init("http://otel-col:4318/v1/traces")

span = Span("name")
time.sleep(1)

span.SetAttribute("test.int", 11)
span.SetAttribute("test.double", 99.99)
span.SetAttribute("test.bool", True)

time.sleep(1)

ctx = span.GetContext()

traceparent = ctx.Inject()
print(inject)

ctx2 = Context.Extract(traceparent, "")

span2 = Span(ctx2, "span2 ext")
time.sleep(1)
span2.SetAttribute("test.string", "mystring")
span2.SetAttribute("test.int", 777)
span2.End()
time.sleep(1)
time.sleep(1)
span.End()

