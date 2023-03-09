#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <typeinfo>
#include <any>
#include "opt.h"

static std::string _profilName = "";
static std::string _version = "1.2.0";
static std::string _schema = "https://opentelemetry.io/schemas/1.2.0";

namespace nostd	= opentelemetry::nostd;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace common = opentelemetry::common;
namespace metrics_exporter = opentelemetry::exporter::metrics;
namespace metrics_api = opentelemetry::metrics;

/// SetLogLevel
static std::shared_mutex _mutex;
static std::map<std::string, std::optional<std::type_index>> _mapType;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<double>>> _mapGauge;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>> _mapGaugeLastValue;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<double>>> _mapCounter;
static std::map<std::string, opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>>> _mapHistogram;

void OPT::Shutdown()
{
	_profilName = "";
	std::shared_ptr<metrics_api::MeterProvider> none;
	metrics_api::Provider::SetMeterProvider(none);
}

OPT::OPT(const char* url, const char* profilName)
{
	if (url == NULL || strcmp(url, "") == 0) {
		// Error
		return;
	}
	if (profilName == NULL || strcmp(profilName, "") == 0) {
		// Error
		return;
	}
	if (_profilName != "") {
		// Error
		return;
	}

	_profilName = std::string(profilName);

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

void OPT::CreateMetricGauge(const char* _name, const char* description)
{
	std::unique_lock lock(_mutex);
	if (_name == NULL || strcmp(_name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string name = std::string(_name);
	std::string gauge_name = name + std::string("_gauge");
	std::string gauge_description = std::string(description);
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

void OPT::CreateMetricGaugeLastValue(const char* _name, const char* description)
{
	std::unique_lock lock(_mutex);
	if (_name == NULL || strcmp(_name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string name = std::string(_name);
	std::string gauge_name = name + std::string("_gauge");
	std::string gauge_description = std::string(description);
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

void OPT::CreateMetricCounter(const char* _name, const char* description)
{
	std::unique_lock lock(_mutex);
	if (_name == NULL || strcmp(_name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string name = std::string(_name);
	std::string counter_name = name + std::string("_counter");
	std::string counter_description = std::string(description);
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

void OPT::CreateMetricHistogram(const char* _name, const char* description)
{
	std::unique_lock lock(_mutex);
	if (_name == NULL || strcmp(_name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	// Already exist
	std::string name = std::string(_name);
	std::string histogram_name = name + std::string("_histogram");
	std::string histogram_description = std::string(description);
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

void OPT::UpdateMeterGaugeAdd(const char* name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name == NULL || strcmp(name, "") == 0) {
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

void OPT::UpdateMeterGaugeLastValueSet(const char* name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name == NULL || strcmp(name, "") == 0) {
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

void OPT::UpdateMeterCounterAdd(const char* name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name == NULL || strcmp(name, "") == 0) {
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

void OPT::UpdateMeterHistogramRecord(const char* name, double val, std::map<std::string, std::string> labels)
{
	std::unique_lock lock(_mutex);
	if (_profilName == "") {
		// Error
		return;
	}
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	std::string histogram_name = std::string(name) + std::string("_histogram");
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

void OPT::UpdateMeterGaugeAdd(const char* name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterGaugeAdd(name, val, labels);
}

void OPT::UpdateMeterGaugeLastValueSet(const char* name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterGaugeLastValueSet(name, val, labels);
}

void OPT::UpdateMeterCounterAdd(const char* name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterCounterAdd(name, val, labels);
}

void OPT::UpdateMeterHistogramRecord(const char* name, double val)
{
	std::map<std::string, std::string> labels;
	UpdateMeterHistogramRecord(name, val, labels);
}

