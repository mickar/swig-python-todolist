#ifndef OPENTELEMTRY_PROMETHEUS_PYTHON_H
#define OPENTELEMTRY_PROMETHEUS_PYTHON_H

#include <any>

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
#include <opentelemetry/trace/propagation/http_trace_context.h>

enum class LogLevel
{
	Error = 0,
	Warning,
	Info,
	Debug
};

class Context {
	private:
		static constexpr uint8_t kInvalidVersion = 0xFF;
		static bool IsValidVersion(opentelemetry::nostd::string_view version_hex);
		static opentelemetry::trace::TraceId TraceIdFromHex(opentelemetry::nostd::string_view trace_id);
		static opentelemetry::trace::SpanId SpanIdFromHex(opentelemetry::nostd::string_view span_id);
		static opentelemetry::trace::TraceFlags TraceFlagsFromHex(opentelemetry::nostd::string_view trace_flags);
		//static constexpr int TraceIDSize = 16;
		//static constexpr int SpanIDSize = 16;
		opentelemetry::trace::SpanContext _spanContext = opentelemetry::trace::SpanContext(false, false);
	public:
		Context(const opentelemetry::trace::SpanContext &ctx);
		Context(const std::string &traceParent, const std::string &traceState);
		std::string Inject();
		static Context Extract(const std::string &traceParent, const std::string &traceState);

		opentelemetry::trace::SpanContext GetOpentelemetryContext(void) const;
};

class Span {
	private:
		opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span> _span;
	public:
		Span(const Context &ctx, const std::string name);
		Span(const std::string name);

		Context GetContext(void);
		void End(void);

		void SetAttribute(const std::string &key, bool value);
		void SetAttribute(const std::string &key, int32_t value);
		void SetAttribute(const std::string &key, uint32_t value);
		void SetAttribute(const std::string &key, int64_t value);
		void SetAttribute(const std::string &key, uint64_t value);
		void SetAttribute(const std::string &key, double value);
		void SetAttribute(const std::string &key, const std::string &value);
};

class Traces {
	private:
		static void SampleFunction1();
		static void SampleFunction2();
	protected:
	public:
		static opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer();
		static void Init(const std::string &url);

		// Sample
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
		static void Debug(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Info(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Warn(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Error(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Debug(const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Info(const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Warn(const std::string &body, std::map<std::string, std::string> attributes = {});
		static void Error(const std::string &body, std::map<std::string, std::string> attributes = {});
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

