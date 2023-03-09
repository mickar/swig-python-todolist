#ifndef OPENTELEMTRY_PROMETHEUS_PYTHON_H
#define OPENTELEMTRY_PROMETHEUS_PYTHON_H

#include <opentelemetry/exporters/prometheus/exporter.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/context/context.h>
#include <opentelemetry/nostd/shared_ptr.h>

class OPT {
	private:
		std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> _p;
	public:
		OPT(const char* url, const char* profilName);
		~OPT();

		//static void Init(const char* url, const char* profilName);
		static void Shutdown();

		// Create metric
		void CreateMetricGauge(const char* name, const char* description);
		void CreateMetricGaugeLastValue(const char* name, const char* description);
		void CreateMetricCounter(const char* name, const char* description);
		void CreateMetricHistogram(const char* name, const char* description);

		// Update
		static void UpdateMeterGaugeAdd(const char* name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterGaugeLastValueSet(const char* name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterCounterAdd(const char* name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterHistogramRecord(const char* name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterGaugeAdd(const char* name, double val);
		static void UpdateMeterGaugeLastValueSet(const char* name, double val);
		static void UpdateMeterCounterAdd(const char* name, double val);
		static void UpdateMeterHistogramRecord(const char* name, double val);
};

#endif

