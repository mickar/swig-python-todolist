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

#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/version/version.h>
#include <opentelemetry/trace/provider.h>


enum class LogLevel
{
	Error = 0,
	Warning,
	Info,
	Debug
};

class Traces {
	private:
		static void SampleFunction1();
		static void SampleFunction2();
	protected:
		static opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer();
	public:
		static void Init(const std::string &url);
		static void SampleCreateSpan();
		static void SampleErrorCreateSpan();
		static void SampleScopeCreateSpan();
};

class Logs {
	private:
	protected:
		static opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger();
	public:
		static void Init(const std::string &url);
		static void SetLogLevel(LogLevel &l);
		static void Info(void);
		static void Error(void);
};

class Metrics {
	private:
		std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> _p;
	public:
		Metrics(const std::string &url, const std::string &profilName);
		~Metrics(){};

		//static void Init(const std::string &url, const std::string &profilName);
		static void Shutdown();

		// Create metric
		void CreateMetricGauge(const std::string &name, const std::string &description);
		void CreateMetricGaugeLastValue(const std::string &name, const std::string &description);
		void CreateMetricCounter(const std::string &name, const std::string &description);
		void CreateMetricHistogram(const std::string &name, const std::string &description);

		// Update
		static void UpdateMeterGaugeAdd(const std::string &name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterGaugeLastValueSet(const std::string &name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterCounterAdd(const std::string &name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterHistogramRecord(const std::string &name, double val, std::map<std::string, std::string> labels);
		static void UpdateMeterGaugeAdd(const std::string &name, double val);
		static void UpdateMeterGaugeLastValueSet(const std::string &name, double val);
		static void UpdateMeterCounterAdd(const std::string &name, double val);
		static void UpdateMeterHistogramRecord(const std::string &name, double val);
};

#endif

