from todolist import *
import time


metrics = Metrics("todolist:8081", "testprofile")
metrics.CreateMetricGauge("test1", "desc test1")
Metrics.UpdateMeterGaugeAdd("test1", 11)
Metrics.UpdateMeterGaugeAdd("test1", 13)
Metrics.UpdateMeterGaugeAdd("test1", 13, OpentelemetryLabels({"label1":"valueA","label2":"valueB"}))

time.sleep(10)

