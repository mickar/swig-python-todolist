#include <memory>
#include <unistd.h>
#include "opt.h"

int main() {
	auto opt = new OPT("127.0.0.1:8081", "opt_todolist");
	opt->CreateMetricGauge("mygaugemeter", "DESC Gauge with UP and DOWN");
	opt->CreateMetricGaugeLastValue("mygaugelastvaluemeter", "DESC Gauge with Last Value");
	opt->CreateMetricCounter("mycountermeter", "DESC Gauge with UP and DOWN");

	sleep(1);

	OPT::UpdateMeterGaugeAdd("mygaugemeter", 777);
	OPT::UpdateMeterGaugeAdd("mygaugemeter", 778);
	OPT::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 66);
	OPT::UpdateMeterGaugeLastValueSet("mygaugelastvaluemeter", 55);
	OPT::UpdateMeterCounterAdd("mycountermeter", 100);
	OPT::UpdateMeterCounterAdd("mycountermeter", 100);

	sleep(15);

	return 0;
}

