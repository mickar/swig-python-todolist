#include <memory>
#include <unistd.h>
#include "opt.h"

int main() {
	std::cout << "Init" << std::endl;

	OPT::Init("127.0.0.1:8081", "opt_todolist");
	OPT::CreateMetricGauge("mygaugemeter", "DESC Gauge with UP and DOWN");
	OPT::CreateMetricGaugeLastValue("mygaugelastvaluemeter", "DESC Gauge with Last Value");
	OPT::CreateMetricCounter("mycountermeter", "DESC Gauge with UP and DOWN");

	sleep(1);

	std::cout << "Updates metrics" << std::endl;
	OPT::UpdateMeterGaugeAdd("mygaugemeter", 777);
	OPT::UpdateMeterGaugeAdd("mygaugemeter", -77);
	OPT::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 66);
	OPT::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 55);
	OPT::UpdateMeterCounterAdd("mycountermeter", 100);
	OPT::UpdateMeterCounterAdd("mycountermeter", 100);

	sleep(5);

	std::cout << "Shutdown" << std::endl;
	OPT::Shutdown();

	sleep(5);

	return 0;
}

