#include <memory>
#include "opt.h"

static std::string _profilName = "otp_todolist";
static std::string _version = "1.2.0";
static std::string _schema = "https://opentelemetry.io/schemas/1.2.0";

namespace nostd	= opentelemetry::nostd;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace common = opentelemetry::common;
namespace metrics_exporter = opentelemetry::exporter::metrics;
namespace metrics_api = opentelemetry::metrics;

/// SetLogLevel

OPT::~OPT() {}

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
	   //this->provider = metrics_api::Provider::GetMeterProvider();
		//this->p = std::static_pointer_cast<metrics_sdk::MeterProvider>(this->provider);
		//this->meter = provider->GetMeter(_profilName, "1.2.0");
		return;
	 }

	 _profilName = std::string(profilName);

	 // Set URL on opts
	 opentelemetry::exporter::metrics::PrometheusExporterOptions opts;
	 opts.url = std::string(url);
/*
	 // Init exporter and reader
	 auto exporter = std::unique_ptr<metrics_sdk::PushMetricExporter>{new metrics_exporter::PrometheusExporter(opts)};
	 opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;
	 options.export_interval_millis = std::chrono::milliseconds(2000);
	 options.export_timeout_millis= std::chrono::milliseconds(1000);
	 auto reader = std::unique_ptr<metrics_sdk::MetricReader>{new metrics_sdk::PeriodicExportingMetricReader(std::move(exporter), options)};

	 // Initialize provider and set the global MeterProvider
	 auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metrics_sdk::MeterProvider());
	 this->p = std::static_pointer_cast<metrics_sdk::MeterProvider>(provider);
	 p->AddMetricReader(std::move(reader));
*/
  std::shared_ptr<metrics_sdk::MetricReader> prometheus_exporter(
      new metrics_exporter::PrometheusExporter(opts));
  auto provider = std::shared_ptr<metrics_api::MeterProvider>(new metrics_sdk::MeterProvider());
  this->p        = std::static_pointer_cast<metrics_sdk::MeterProvider>(provider);
  this->p->AddMetricReader(prometheus_exporter);
  metrics_api::Provider::SetMeterProvider(provider);
}

void OPT::CreateMetricGauge(const char* name, const char* description)
{
	if (name == NULL || strcmp(name, "") == 0) {
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		return;
	}
	std::string gauge_name = std::string(name) + std::string("_gauge");
	std::string gauge_description = std::string(description);
	std::unique_ptr<metrics_sdk::InstrumentSelector> gauge_instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kUpDownCounter, gauge_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> gauge_meter_selector{
		new metrics_sdk::MeterSelector(gauge_name, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> gauge_sum_view{
		new metrics_sdk::View{gauge_name, gauge_description, metrics_sdk::AggregationType::kSum}};
	this->p->AddView(std::move(gauge_instrument_selector), std::move(gauge_meter_selector), std::move(gauge_sum_view));
}

void OPT::CreateMetricGaugeLastValue(const char* name, const char* description)
{
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string gauge_name = std::string(name) + std::string("_gauge");
	std::string gauge_description = std::string(description);
	std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kCounter, gauge_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
		new metrics_sdk::MeterSelector(gauge_name, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> sum_view{
		new metrics_sdk::View{gauge_name, gauge_description, metrics_sdk::AggregationType::kLastValue}};
	this->p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
}

void OPT::CreateMetricCounter(const char* name, const char* description)
{
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string counter_name = std::string(name) + std::string("_counter");
	std::string counter_description = std::string(description);
	std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kCounter, counter_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
		new metrics_sdk::MeterSelector(counter_name, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> sum_view{
		new metrics_sdk::View{counter_name, counter_description, metrics_sdk::AggregationType::kSum}};
	this->p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));

}

void OPT::CreateMetricHistogram(const char* name, const char* description)
{
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	if (description == NULL || strcmp(description, "") == 0) {
		// Error
		return;
	}
	std::string histogram_name = std::string(name) + std::string("_histogram");
	std::string histogram_description = std::string(description);
	std::unique_ptr<metrics_sdk::InstrumentSelector> histogram_instrument_selector{
		new metrics_sdk::InstrumentSelector(metrics_sdk::InstrumentType::kHistogram, histogram_name)};
	std::unique_ptr<metrics_sdk::MeterSelector> histogram_meter_selector{
		new metrics_sdk::MeterSelector(histogram_name, _version, _schema)};
	std::unique_ptr<metrics_sdk::View> histogram_view{
		new metrics_sdk::View{histogram_name, histogram_description, metrics_sdk::AggregationType::kHistogram}};
	this->p->AddView(std::move(histogram_instrument_selector), std::move(histogram_meter_selector), std::move(histogram_view));
}

void OPT::UpdateMeterGaugeAdd(const char* name, double val, std::map<std::string, std::string> labels)
{
	 if (_profilName == "") {
		// Error
		return;
	 }
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	 // Init
	 auto provider = metrics_api::Provider::GetMeterProvider();
	 auto meter = provider->GetMeter(_profilName, _version);
	 // Update
	std::string gauge_name = std::string(name) + std::string("_gauge");
	auto context = opentelemetry::context::Context{};
	auto double_gauge = meter->CreateDoubleUpDownCounter(gauge_name);
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	double_gauge->Add(val, labelkv, context);
}

void OPT::UpdateMeterGaugeLastValueSet(const char* name, double val, std::map<std::string, std::string> labels)
{
	 if (_profilName == "") {
		// Error
		return;
	 }
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	 // Init
	 auto provider = metrics_api::Provider::GetMeterProvider();
	 auto meter = provider->GetMeter(_profilName, _version);
	 // Update value
	std::string gauge_name = std::string(name) + std::string("_gauge");
	auto context = opentelemetry::context::Context{};
	auto double_gauge = meter->CreateDoubleUpDownCounter(gauge_name);
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	double_gauge->Add(val, labelkv, context);

}

void OPT::UpdateMeterCounterAdd(const char* name, double val, std::map<std::string, std::string> labels)
{
	 if (_profilName == "") {
		// Error
		return;
	 }
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	 // Init
	 auto provider = metrics_api::Provider::GetMeterProvider();
	 auto meter = provider->GetMeter(_profilName, _version);
	 // Update value
	std::string counter_name = std::string(name) + std::string("_counter");
	auto context = opentelemetry::context::Context{};
	auto double_counter = meter->CreateDoubleCounter(counter_name);
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	double_counter->Add(val, labelkv, context);
}

void OPT::UpdateMeterHistogramRecord(const char* name, double val, std::map<std::string, std::string> labels)
{
	 if (_profilName == "") {
		// Error
		return;
	 }
	if (name == NULL || strcmp(name, "") == 0) {
		// Error
		return;
	}
	// Init
	auto provider = metrics_api::Provider::GetMeterProvider();
	auto meter = provider->GetMeter(_profilName, _version);
	// Update value
	std::string histogram_name = std::string(name) + std::string("_histogram");
	auto context = opentelemetry::context::Context{};
	auto histogram_counter = meter->CreateDoubleHistogram(histogram_name);
	auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
	histogram_counter->Record(val, labelkv, context);
}
/*
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
*/
