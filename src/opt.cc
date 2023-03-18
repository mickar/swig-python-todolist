#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <typeinfo>
#include <any>
#include "opt.h"

static std::string _profilName = "";
static std::string _version = "1.2.0";
static std::string _schema = "https://opentelemetry.io/schemas/1.19.0";

namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace common = opentelemetry::common;
namespace metrics_exporter = opentelemetry::exporter::metrics;
namespace metrics_api = opentelemetry::metrics;


#include <opentelemetry/exporters/otlp/otlp_http_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor_factory.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/propagation/detail/hex.h>
#include <opentelemetry/trace/propagation/detail/string.h>

///////////////////////////
// Tracing
///////////////////////////

//////////////
// Context
/////////////

Context::Context(const opentelemetry::trace::SpanContext &ctx)
{
	this->_spanContext = ctx;
}

Context Context::Extract(const std::string &traceParent, const std::string &traceState)
{
	return Context(traceParent, traceState);
}

Context::Context(const std::string &traceParent, const std::string &traceState)
{
	if (traceParent.size() != opentelemetry::trace::propagation::kTraceParentSize)
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	std::array<opentelemetry::nostd::string_view, 4> fields{};
	if (opentelemetry::trace::propagation::detail::SplitString(traceParent, '-', fields.data(), 4) != 4)
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	opentelemetry::nostd::string_view versionHex    = fields[0];
	opentelemetry::nostd::string_view traceIDHex    = fields[1];
	opentelemetry::nostd::string_view spanIDHex     = fields[2];
	opentelemetry::nostd::string_view traceFlagsHex = fields[3];

	if (versionHex.size() != opentelemetry::trace::propagation::kVersionSize || traceIDHex.size() != opentelemetry::trace::propagation::kTraceIdSize ||
			spanIDHex.size() != opentelemetry::trace::propagation::kSpanIdSize || traceFlagsHex.size() != opentelemetry::trace::propagation::kTraceFlagsSize)
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	if (!opentelemetry::trace::propagation::detail::IsValidHex(versionHex) || !opentelemetry::trace::propagation::detail::IsValidHex(traceIDHex) ||
			!opentelemetry::trace::propagation::detail::IsValidHex(spanIDHex) || !opentelemetry::trace::propagation::detail::IsValidHex(traceFlagsHex))
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	if (!IsValidVersion(versionHex))
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	opentelemetry::trace::TraceId traceID = TraceIdFromHex(traceIDHex);
	opentelemetry::trace::SpanId spanID   = SpanIdFromHex(spanIDHex);

	if (!traceID.IsValid() || !spanID.IsValid())
	{
		// Error
		this->_spanContext = opentelemetry::trace::SpanContext::GetInvalid();
	}

	this->_spanContext = opentelemetry::trace::SpanContext(traceID, spanID, TraceFlagsFromHex(traceFlagsHex), true, opentelemetry::trace::TraceState::FromHeader(traceState));
}

opentelemetry::trace::SpanContext Context::GetOpentelemetryContext() const
{
	return this->_spanContext;
}

opentelemetry::trace::TraceId Context::TraceIdFromHex(opentelemetry::nostd::string_view traceID)
{
	uint8_t buf[opentelemetry::trace::propagation::kTraceIdSize / 2];
	opentelemetry::trace::propagation::detail::HexToBinary(traceID, buf, sizeof(buf));
	return opentelemetry::trace::TraceId(buf);
}

opentelemetry::trace::SpanId Context::SpanIdFromHex(opentelemetry::nostd::string_view spanID)
{
	uint8_t buf[opentelemetry::trace::propagation::kSpanIdSize / 2];
	opentelemetry::trace::propagation::detail::HexToBinary(spanID, buf, sizeof(buf));
	return opentelemetry::trace::SpanId(buf);
}

opentelemetry::trace::TraceFlags Context::TraceFlagsFromHex(opentelemetry::nostd::string_view traceFlags)
{
	uint8_t flags;
	opentelemetry::trace::propagation::detail::HexToBinary(traceFlags, &flags, sizeof(flags));
	return opentelemetry::trace::TraceFlags(flags);
}

bool Context::IsValidVersion(opentelemetry::nostd::string_view versionHex)
{
	uint8_t version;
	opentelemetry::trace::propagation::detail::HexToBinary(versionHex, &version, sizeof(version));
	return version != kInvalidVersion;
}

std::string Context::Inject()
{
	char traceParent[opentelemetry::trace::propagation::kTraceParentSize];
	traceParent[0] = '0';
	traceParent[1] = '0';
	traceParent[2] = '-';
	_spanContext.trace_id().ToLowerBase16( opentelemetry::nostd::span<char, 2 * opentelemetry::trace::TraceId::kSize>{&traceParent[3], opentelemetry::trace::propagation::kTraceIdSize});
	traceParent[opentelemetry::trace::propagation::kTraceIdSize + 3] = '-';
	_spanContext.span_id().ToLowerBase16( opentelemetry::nostd::span<char, 2 * opentelemetry::trace::SpanId::kSize>{&traceParent[opentelemetry::trace::propagation::kTraceIdSize + 4], opentelemetry::trace::propagation::kSpanIdSize});
	traceParent[opentelemetry::trace::propagation::kTraceIdSize + opentelemetry::trace::propagation::kSpanIdSize + 4] = '-';
	_spanContext.trace_flags().ToLowerBase16( opentelemetry::nostd::span<char, 2>{&traceParent[opentelemetry::trace::propagation::kTraceIdSize + opentelemetry::trace::propagation::kSpanIdSize + 5], 2});
	return std::string(traceParent);
}

//////////////
// Span
/////////////

Span::Span(const Context &ctx, const std::string name)
{
	auto tracer = Traces::GetTracer();
	if (tracer == nullptr) {
		// Error
		return;
	}
	opentelemetry::trace::StartSpanOptions options;
	options.parent = ctx.GetOpentelemetryContext();
	this->_span = tracer->StartSpan(name, options);
}

Span::Span(const std::string name)
{
	auto tracer = Traces::GetTracer();
	if (tracer == nullptr) {
		// Error
		return;
	}
	this->_span = tracer->StartSpan(name);
}

Context Span::GetContext()
{
	return Context(this->_span->GetContext());
}

void Span::End()
{
	this->_span->End();
}

void Span::SetAttribute(const std::string &key, bool value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, int32_t value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, uint32_t value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, int64_t value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, uint64_t value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, double value)
{
	this->_span->SetAttribute(key, value);
}

void Span::SetAttribute(const std::string &key, const std::string &value)
{
	this->_span->SetAttribute(key, value);
}

//////////////
// Traces
/////////////

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> Traces::GetTracer()
{
	auto provider = opentelemetry::trace::Provider::GetTracerProvider();
	return provider->GetTracer("foo_library_trace", OPENTELEMETRY_SDK_VERSION, "test");
}

void Traces::SampleFunction1()
{
  auto scoped_span = opentelemetry::trace::Scope(Traces::GetTracer()->StartSpan("Span Scope SampleFunction1"));
}

void Traces::SampleFunction2()
{
  auto scoped_span = opentelemetry::trace::Scope(Traces::GetTracer()->StartSpan("Span Scope SampleFunction2"));

  SampleFunction1();
  SampleFunction1();
}


void Traces::SampleScopeCreateSpan()
{
	auto scoped_span = opentelemetry::trace::Scope(Traces::GetTracer()->StartSpan("span scope main funtion"));
	SampleFunction2();
	SampleFunction2();
}

void Traces::SampleErrorCreateSpan()
{
	auto span_first  = Traces::GetTracer()->StartSpan("span err 1");

	span_first->SetAttribute("attr1", 3.1);

	opentelemetry::trace::StartSpanOptions options;
	options.parent   = span_first->GetContext();
	auto span_second = Traces::GetTracer()->StartSpan("span err 2", options);

	options.parent  = span_second->GetContext();
	auto span_third = Traces::GetTracer()->StartSpan("span err 3", options);

	span_second->SetStatus(opentelemetry::trace::StatusCode::kError, "emergency error");

	span_third->End();
	span_second->End();
	span_first->End();
}

void Traces::SampleCreateSpan()
{
	auto span_first  = Traces::GetTracer()->StartSpan("span 1");

	span_first->SetAttribute("attr1", 5.3);

	opentelemetry::trace::StartSpanOptions options;
	options.parent   = span_first->GetContext();
	auto span_second = Traces::GetTracer()->StartSpan("span 2", options);

	options.parent  = span_second->GetContext();
	auto span_third = Traces::GetTracer()->StartSpan("span 3", options);

	span_second->SetStatus(opentelemetry::trace::StatusCode::kOk, "everything is awesome !!!!!!!!!!!!");
	span_second->SetAttribute("otel.status_description", "everything is awesome !!");

	span_third->End();
	span_second->End();
	span_first->End();
}

void Traces::Init(const std::string &url)
{
	opentelemetry::sdk::resource::ResourceAttributes attributes = {
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkLanguage, "cpp"},
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkName, "opentelemetry"},
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
		{opentelemetry::sdk::resource::SemanticConventions::kServiceName, "todolist"},
		{"my.custom.label", "custom"},
	};
	auto resource = opentelemetry::sdk::resource::Resource::Create(attributes);

	opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;
	if (url.compare("") == 0) {
		// Error
		return;
	}
	opts.url = url;
	// Create OTLP exporter instance
	auto exporter  = opentelemetry::exporter::otlp::OtlpHttpExporterFactory::Create(opts);
	auto processor = opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));
	std::shared_ptr<opentelemetry::trace::TracerProvider> provider = opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor), resource);
	// Set the global trace provider
	opentelemetry::trace::Provider::SetTracerProvider(provider);
}

///////////////////////////
// Logs
///////////////////////////

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> Logs::GetLogger()
{
	auto provider = opentelemetry::logs::Provider::GetLoggerProvider();
	std::unordered_map<std::string, std::string> attributes = {
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkLanguage, "cpp"},
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkName, "opentelemetry"},
		{opentelemetry::sdk::resource::SemanticConventions::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION},
		{opentelemetry::sdk::resource::SemanticConventions::kServiceName, "todolist"},
		{"my.custom.label", "custom"},
	};
	return provider->GetLogger("todolist", "opentelelemtry_library", "1.19.0", _schema, true, attributes);
}


void Logs::Init(const std::string &url)
{
	opentelemetry::exporter::otlp::OtlpHttpLogRecordExporterOptions opts;
	if (url.compare("") == 0) {
		// Error
		return;
	}
	opts.url = url;
	opts.console_debug = true;
	// Create OTLP exporter instance
	auto exporter  = opentelemetry::exporter::otlp::OtlpHttpLogRecordExporterFactory::Create(opts);
	auto processor = opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
	std::shared_ptr<opentelemetry::logs::LoggerProvider> provider = opentelemetry::sdk::logs::LoggerProviderFactory::Create(std::move(processor));

	opentelemetry::logs::Provider::SetLoggerProvider(provider);

	opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(opentelemetry::sdk::common::internal_log::LogLevel::Debug);
}

void Logs::SetLogLevel(LogLevel &l)
{
	std::cout << "ok debug" << std::endl;
	opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(opentelemetry::sdk::common::internal_log::LogLevel::Debug);
}

void Logs::Error(const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kError, body,
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Warn(const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kWarn, body,
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Info(const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, body,
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Debug(const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kDebug, body,
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Error(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kError, body,
			ctx.GetOpentelemetryContext().trace_id(), ctx.GetOpentelemetryContext().span_id(), ctx.GetOpentelemetryContext().trace_flags(),
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Warn(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kWarn, body,
			ctx.GetOpentelemetryContext().trace_id(), ctx.GetOpentelemetryContext().span_id(), ctx.GetOpentelemetryContext().trace_flags(),
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Info(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, body,
			ctx.GetOpentelemetryContext().trace_id(), ctx.GetOpentelemetryContext().span_id(), ctx.GetOpentelemetryContext().trace_flags(),
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}

void Logs::Debug(const Context &ctx, const std::string &body, std::map<std::string, std::string> attributes)
{
	auto logger = Logs::GetLogger();
	logger->EmitLogRecord(opentelemetry::logs::Severity::kDebug, body,
			ctx.GetOpentelemetryContext().trace_id(), ctx.GetOpentelemetryContext().span_id(), ctx.GetOpentelemetryContext().trace_flags(),
			opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()), attributes);
}


///////////////////////////
// Metrics
///////////////////////////

/// SetLogLevel
static std::shared_mutex _mutex;
static std::map<std::string, std::optional<std::type_index>> _mapType;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<double>>> _mapGauge;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>> _mapGaugeLastValue;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>> _mapCounter;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>>> _mapHistogram;

void Metrics::Shutdown()
{
	_profilName = "";
	std::shared_ptr<metrics_api::MeterProvider> none;
	metrics_api::Provider::SetMeterProvider(none);
}

Metrics::Metrics(const std::string &url, const std::string &profilName)
{
	if (url.compare("") == 0) {
		// Error
		return;
	}
	if (profilName.compare("") == 0) {
		// Error
		return;
	}
	if (_profilName.compare("") != 0) {
		// Error
		return;
	} else {
		_profilName = profilName;
	}

	// Set URL on opts
	metrics_exporter::PrometheusExporterOptions opts;
	opts.url = std::string(url);

	// Init exporter and reader
	std::unique_ptr<metrics_sdk::PushMetricExporter> exporter{new metrics_exporter::PrometheusExporter(opts)};
	opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;
	options.export_interval_millis = std::chrono::milliseconds(2000);
	options.export_timeout_millis= std::chrono::milliseconds(1000);
	std::unique_ptr<metrics_sdk::MetricReader> reader{new metrics_sdk::PeriodicExportingMetricReader(std::move(exporter), options)};

	// Initialize provider and set the global MeterProvider
	auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metrics_sdk::MeterProvider());
	_p = std::static_pointer_cast<metrics_sdk::MeterProvider>(provider);
	_p->AddMetricReader(std::move(reader));
	metrics_api::Provider::SetMeterProvider(std::move(provider));
}

void Metrics::CreateMetricGauge(const std::string &name, const std::string &gauge_description)
{
	std::unique_lock lock(_mutex);
	if (name.compare("") == 0) {
		// Error
		return;
	}
	if (gauge_description.compare("") == 0) {
		// Error
		return;
	}
	std::string gauge_name = name + std::string("_gauge");
	// Create view
	std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kUpDownCounter, gauge_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
		new metrics_sdk::MeterSelector(_profilName, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> sum_view{
		new metrics_sdk::View{gauge_name, gauge_description, metrics_sdk::AggregationType::kSum}};
	_p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
	// Create and save meter
	auto _provider = metrics_api::Provider::GetMeterProvider();
	auto _meter = _provider->GetMeter(_profilName, _version);
	opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<double>> double_gauge = _meter->CreateDoubleUpDownCounter(gauge_name);
	_mapType[name] = typeid(double_gauge);
	_mapGauge[name] = std::move(double_gauge);
}

void Metrics::CreateMetricGaugeLastValue(const std::string &name, const std::string &gauge_description)
{
	std::unique_lock lock(_mutex);
	if (name.compare("") == 0) {
		// Error
		return;
	}
	if (gauge_description.compare("") == 0) {
		// Error
		return;
	}
	std::string gauge_name = name + std::string("_gauge");
	// Create view
	std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kCounter, gauge_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
		new metrics_sdk::MeterSelector(_profilName, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> sum_view{
		new metrics_sdk::View{gauge_name, gauge_description, metrics_sdk::AggregationType::kLastValue}};
	_p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
	// Create and save meter
	auto _provider = metrics_api::Provider::GetMeterProvider();
	auto _meter = _provider->GetMeter(_profilName, _version);
	opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>> double_gauge = _meter->CreateDoubleCounter(gauge_name);
	_mapType[name] = typeid(double_gauge);
	_mapGaugeLastValue[name] = std::move(double_gauge);
}

void Metrics::CreateMetricCounter(const std::string &name, const std::string &counter_description)
{
	std::unique_lock lock(_mutex);
	if (name.compare("") == 0) {
		// Error
		return;
	}
	if (counter_description.compare("") == 0) {
		// Error
		return;
	}
	std::string counter_name = name + std::string("_counter");
	// Create view
	std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kCounter, counter_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
		new metrics_sdk::MeterSelector(_profilName, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> sum_view{
		new metrics_sdk::View{counter_name, counter_description, metrics_sdk::AggregationType::kSum}};
	_p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
	// Create and save meter
	auto _provider = metrics_api::Provider::GetMeterProvider();
	auto _meter = _provider->GetMeter(_profilName, _version);
	opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>> double_counter = _meter->CreateDoubleCounter(counter_name);
	_mapType[name] = typeid(double_counter);
	_mapCounter[name] = std::move(double_counter);
}

void Metrics::CreateMetricHistogram(const std::string &name, const std::string &histogram_description)
{
	std::unique_lock lock(_mutex);
	if (name.compare("") == 0) {
		// Error
		return;
	}
	if (histogram_description.compare("") == 0) {
		// Error
		return;
	}
	// Already exist
	std::string histogram_name = name + std::string("_histogram");
	std::unique_ptr<metrics_sdk::InstrumentSelector> histogram_instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kHistogram, histogram_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> histogram_meter_selector{
		new metrics_sdk::MeterSelector(_profilName, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> histogram_view{
		new metrics_sdk::View{histogram_name, histogram_description, metrics_sdk::AggregationType::kHistogram}};
	_p->AddView(std::move(histogram_instrument_selector), std::move(histogram_meter_selector), std::move(histogram_view));
	// Create and save meter
	auto _provider = metrics_api::Provider::GetMeterProvider();
	auto _meter = _provider->GetMeter(_profilName, _version);
	opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>> double_histogram = _meter->CreateDoubleHistogram(histogram_name);
	_mapType[name] = typeid(double_histogram);
	_mapHistogram[name] = std::move(double_histogram);
}

void Metrics::UpdateMeterGaugeAdd(const std::string &name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name.compare("") == 0) {
		// Error
		return;
	}
	std::string gauge_name = std::string(name) + std::string("_gauge");
	// Init somes vars
	auto context = opentelemetry::context::Context{};
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	// Update
	auto type = _mapType[name];
	if (type.value_or(typeid(int)).hash_code() != typeid(opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<double>>).hash_code()) {
		// Error
		return;
	}
	auto search = _mapGauge.find(name);
	if (search == _mapGauge.end()) {
		// Error
		return;
	}
	search->second->Add(val, labelkv, context);
}

void Metrics::UpdateMeterGaugeLastValueSet(const std::string &name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name.compare("") == 0) {
		// Error
		return;
	}
	std::string gauge_name = std::string(name) + std::string("_gauge");
	// Init somes vars
	auto context = opentelemetry::context::Context{};
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	// Update
	auto type = _mapType[name];
	if (type.value_or(typeid(int)).hash_code() != typeid(opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>).hash_code()) {
		// Error
		return;
	}
	auto search = _mapGaugeLastValue.find(name);
	if (search == _mapGaugeLastValue.end()) {
		// Error
		return;
	}
	search->second->Add(val, labelkv, context);

}

void Metrics::UpdateMeterCounterAdd(const std::string &name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name.compare("") == 0) {
		// Error
		return;
	}
	std::string counter_name = std::string(name) + std::string("_counter");
	// Init somes vars
	auto context = opentelemetry::context::Context{};
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	// Update
	auto type = _mapType[name];
	if (type.value_or(typeid(int)).hash_code() != typeid(opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>).hash_code()) {
		// Error
		return;
	}
	auto search = _mapCounter.find(name);
	if (search == _mapCounter.end()) {
		// Error
		return;
	}
	search->second->Add(val, labelkv, context);
}

void Metrics::UpdateMeterHistogramRecord(const std::string &name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name.compare("") == 0) {
		// Error
		return;
	}
	std::string histogram_name = name + std::string("_histogram");
	// Init somes vars
	auto context = opentelemetry::context::Context{};
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	// Update
	auto type = _mapType[name];
	if (type.value_or(typeid(int)).hash_code() != typeid(opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>>).hash_code()) {
		// Error
		return;
	}
	auto search = _mapHistogram.find(name);
	if (search == _mapHistogram.end()) {
		// Error
		return;
	}
	search->second->Record(val, labelkv, context);
}

void Metrics::UpdateMeterGaugeAdd(const std::string &name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterGaugeAdd(name, val, labels);
}

void Metrics::UpdateMeterGaugeLastValueSet(const std::string &name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterGaugeLastValueSet(name, val, labels);
}

void Metrics::UpdateMeterCounterAdd(const std::string &name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterCounterAdd(name, val, labels);
}

void Metrics::UpdateMeterHistogramRecord(const std::string &name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterHistogramRecord(name, val, labels);
}

