#include <memory>
#include <unistd.h>
#include "opt.h"

int main() {
	std::cout << "Init" << std::endl;
{
	auto metrics = new Metrics("127.0.0.1:8081", "metrics_todolist");
	metrics->CreateMetricGauge("mygaugemeter", "DESC Gauge with UP and DOWN");
	metrics->CreateMetricGaugeLastValue("mygaugelastvaluemeter", "DESC Gauge with Last Value");
	metrics->CreateMetricCounter("mycountermeter", "DESC Gauge with UP and DOWN");
}
	sleep(1);

	std::cout << "Updates metrics" << std::endl;
	Metrics::UpdateMeterGaugeAdd("mygaugemeter", 777);
	Metrics::UpdateMeterGaugeAdd("mygaugemeter", -77);
	Metrics::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 66);
	Metrics::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 55);
	Metrics::UpdateMeterCounterAdd("mycountermeter", 100);
	Metrics::UpdateMeterCounterAdd("mycountermeter", 100);

	sleep(5);

	std::cout << "Shutdown" << std::endl;
	Metrics::Shutdown();

	sleep(5);

	return 0;
}

