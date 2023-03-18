from todolist import *
import time

Traces.Init("http://otel-col:4318/v1/traces")
Logs.Init("http://otel-col:4318/v1/traces")

span = Span("span with log")
time.sleep(1)
span.SetAttribute("test.int", 11)
span.SetAttribute("test.double", 99.99)
span.SetAttribute("test.bool", True)
time.sleep(1)

ctx = span.GetContext()
Logs.Info(ctx, "body info")
Logs.Error(ctx, "body error")
Logs.Warn(ctx, "body warning", OpentelemetryLabels({"label1":"valueA","label2":"valueB"}))

time.sleep(1)
span.End()

