#include "CanSensorModule.hpp"
#include "codes/messages/bottle_temperature/temperature_request.hpp"
#include "codes/messages/bottle_temperature/temperature_response.hpp"
#include "codes/messages/bottle_temperature/top_measured_temperature_request.hpp"
#include "codes/messages/bottle_temperature/top_measured_temperature_response.hpp"
#include "codes/messages/bottle_temperature/bottom_measured_temperature_request.hpp"
#include "codes/messages/bottle_temperature/bottom_measured_temperature_response.hpp"
#include "codes/messages/bottle_temperature/top_sensor_temperature_request.hpp"
#include "codes/messages/bottle_temperature/top_sensor_temperature_response.hpp"
#include "codes/messages/bottle_temperature/bottom_sensor_temperature_request.hpp"
#include "codes/messages/bottle_temperature/bottom_sensor_temperature_response.hpp"
#include "codes/messages/mini_oled/clear_custom_text.hpp"
#include "codes/messages/mini_oled/print_custom_text.hpp"
#include "codes/messages/fluorometer/ojip_capture_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_request.hpp"
#include "codes/messages/fluorometer/ojip_completed_response.hpp"
#include "codes/messages/fluorometer/ojip_retrieve_request.hpp"
#include "codes/messages/fluorometer/data_sample.hpp"
#include "codes/messages/fluorometer/fluorometer_config.hpp"
#include "codes/messages/fluorometer/detector_info_request.hpp"
#include "codes/messages/fluorometer/detector_info_response.hpp"
#include "codes/messages/fluorometer/detector_temperature_request.hpp"
#include "codes/messages/fluorometer/detector_temperature_response.hpp"
#include "codes/messages/fluorometer/emitor_info_request.hpp"
#include "codes/messages/fluorometer/emitor_info_response.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <fstream>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for

static const std::string PARAMS_FILE_PATH = "/data/fluorometer/measurement_params.bin";

CanSensorModule::CanSensorModule(std::string uidHex, ICanChannel::Ptr channel) 
    : base({Modules::Sensor, uidHex}, channel), channel(channel) {
}

ModuleID CanSensorModule::id() const {
    return base.id();
}

std::future <float> CanSensorModule::getBottleTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Temperature_request, 
        App_messages::Bottle_temperature::Temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getTopMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_measured_temperature_request, 
        App_messages::Bottle_temperature::Top_measured_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Top_measured_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getBottomMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_measured_temperature_request, 
        App_messages::Bottle_temperature::Bottom_measured_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Bottom_measured_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getTopSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_sensor_temperature_request, 
        App_messages::Bottle_temperature::Top_sensor_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Top_sensor_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <float> CanSensorModule::getBottomSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_sensor_temperature_request, 
        App_messages::Bottle_temperature::Bottom_sensor_temperature_response, 
        float
    >([](App_messages::Bottle_temperature::Bottom_sensor_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future <bool> CanSensorModule::clearCustomText() {
    App_messages::Mini_OLED::Clear_custom_text r;
    return base.set(r);
}

std::future <bool> CanSensorModule::printCustomText(std::string text) {
    std::vector<std::future<bool>> results;

    for (size_t i = 0; i < text.length(); i += App_messages::Mini_OLED::Print_custom_text::max_length) {
        std::string chunk = text.substr(i, App_messages::Mini_OLED::Print_custom_text::max_length);
        App_messages::Mini_OLED::Print_custom_text r(chunk);
        results.push_back(base.set(r));
    }

    return std::async(std::launch::async, [results = std::move(results)]() mutable {
        for (auto& res : results) {
            if (!res.get()) return false;
        }
        return true;
    });
}

uint8_t CanSensorModule::CalculateMeasurementID(uint32_t api_id) {
    return (api_id % 15) + 1;
}

std::future<bool> CanSensorModule::isFluorometerOjipCaptureComplete() {
    return base.get<
        App_messages::Fluorometer::OJIP_completed_request, 
        App_messages::Fluorometer::OJIP_completed_response, 
        bool
    >([](App_messages::Fluorometer::OJIP_completed_response response){
        return response.completed;
    }, 2000);
}

void CanSensorModule::sendCanRequest(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise) {
    App_messages::Fluorometer::OJIP_retrieve_request rawRequest;
    CanID requestId = BaseModule::createId(rawRequest.Type(), Codes::Module::Sensor_module, Codes::Instance::Exclusive, false);

    RequestData requestData(requestId, rawRequest.Export_data());
    ResponseInfo responseInfo;
    responseInfo.timeoutMs = timeoutMs;
    responseInfo.acceptFunction = [](const ResponseData& response) {
        App_messages::Fluorometer::Data_sample rawResponse;
        return response.id.messageType() == static_cast<uint16_t>(rawResponse.Type());
    };
    responseInfo.isDoneFunction = [](const std::vector<ResponseData>& responses) {
        return false;
    };
    responseInfo.successOnTimeout = true;

    CanRequest canRequest(requestData, responseInfo);

    channel->send(canRequest, [this, promise](ICanChannel::Response response) {
        if (response.status == CanRequestStatus::Success || response.status == CanRequestStatus::Timeout) {
            ISensorModule::FluorometerOjipData result;
            bool metadataLoaded = false;

            for (const auto& rr : response.responseData) {
                auto dataCopy = rr.data;
                App_messages::Fluorometer::Data_sample sampleResponse;
                if (sampleResponse.Interpret_data(dataCopy) &&
                    sampleResponse.Measurement_id() == CalculateMeasurementID(last_api_id)) {
                    
                    if (!metadataLoaded) {
                        result.detector_gain = sampleResponse.gain;
                        result.emitor_intensity = sampleResponse.Emitor_intensity();
                        metadataLoaded = true;
                    }

                    ISensorModule::FluorometerSample sample;
                    sample.time_ms = sampleResponse.Time_ms();
                    sample.raw_value = sampleResponse.sample_value;
                    sample.relative_value = sampleResponse.Relative_value();
                    sample.absolute_value = sampleResponse.Absolute_value();
                    result.samples.push_back(sample);
                    result.measurement_id = last_api_id;
                }
            }

            std::sort(result.samples.begin(), result.samples.end(), [](const FluorometerSample& a, const FluorometerSample& b) {
                return a.time_ms < b.time_ms;
            });

            result.timebase = last_timebase;
            result.length_ms = last_length_ms;
            result.required_samples = last_required_samples;
            result.captured_samples = static_cast<int16_t>(result.samples.size());
            result.missing_samples = result.required_samples - result.captured_samples;
            result.read = isRead;

            last_measurement_data = result;

            promise->set_value(result);
        } else {
            promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
        }
    });
}

void CanSensorModule::checkMeasurementCompletion(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise) {
    int attempts = 0;
    const int maxAttempts = 20;
    const std::chrono::milliseconds delay(100);

    while (attempts < maxAttempts) {
        auto future = isFluorometerOjipCaptureComplete();
        if (future.get()) {
            sendCanRequest(timeoutMs, promise);
            return;
        }

        std::this_thread::sleep_for(delay);
        attempts++;
    }

    promise->set_exception(std::make_exception_ptr(std::runtime_error("Measurement did not complete within the expected time")));
}

bool CanSensorModule::readMeasurementParams(const std::string& filePath, MeasurementParams& params) {
    std::ifstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&params), sizeof(params));
        file.close();
        return true;
    }
    return false;
}

bool CanSensorModule::writeMeasurementParams(const std::string& filePath, const MeasurementParams& params) {
    std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&params), sizeof(params));
        file.close();
        return true;
    }
    return false;
}

std::future<ISensorModule::FluorometerOjipData> CanSensorModule::startFluorometerOjipCapture(
    Fluorometer_config::Gain detector_gain,
    Fluorometer_config::Timing sample_timing,
    float emitor_intensity,
    uint16_t length_ms,
    uint16_t samples) {

    uint32_t api_id; 
    uint8_t measurement_id; 

    MeasurementParams params;
    if (readMeasurementParams(PARAMS_FILE_PATH, params)) {
        last_api_id = params.api_id;
        api_id = ++last_api_id;
        measurement_id = CalculateMeasurementID(api_id);
    } else {
        api_id = 1;
        measurement_id = CalculateMeasurementID(api_id);
        last_api_id = api_id;
    }

    last_measurement_data = ISensorModule::FluorometerOjipData{};
    isRead = false;
    last_timebase = sample_timing;

    params.api_id = api_id;
    params.samples = samples;
    params.length_ms = length_ms;
    params.timebase = static_cast<int>(sample_timing);
    params.isRead = isRead;

    if (!writeMeasurementParams(PARAMS_FILE_PATH, params)) {
        throw std::runtime_error("Failed to write measurement parameters file");
    }

    auto timeoutMs = length_ms;
    last_required_samples = samples;
    last_length_ms = length_ms;

    App_messages::Fluorometer::OJIP_capture_request r{
        measurement_id,
        detector_gain,
        sample_timing,
        static_cast<uint8_t>(emitor_intensity * 255),
        length_ms,
        samples
    };

    auto succes = base.set(r);
    if (succes.get()) {
        auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();
        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
        std::thread([this, promise, timeoutMs]() {
            checkMeasurementCompletion(timeoutMs, promise);
        }).detach();
        return promise->get_future();
    } else {
        auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();
        promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to start OJIP capture")));
        return promise->get_future();
    }
}

std::future<ISensorModule::FluorometerOjipData> CanSensorModule::retrieveFluorometerOjipData() {
    auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();

    if (last_api_id == 0) {
        MeasurementParams params;
        if (readMeasurementParams(PARAMS_FILE_PATH, params)) {
            last_api_id = params.api_id;
            last_required_samples = params.samples;
            last_length_ms = params.length_ms;
            last_timebase = static_cast<Fluorometer_config::Timing>(params.timebase);
            isRead = params.isRead;
        } else {
            promise->set_exception(std::make_exception_ptr(std::runtime_error("No measurements have been taken yet")));
            return promise->get_future();
        }
    }

    if (!last_measurement_data.samples.empty()) {
        if (!isRead) {
            last_measurement_data.read = false;
            isRead = true;
        } else {
            last_measurement_data.read = true;
        }

        MeasurementParams params = {last_api_id, last_required_samples, last_length_ms, static_cast<int>(last_timebase), isRead};
        if (!writeMeasurementParams(PARAMS_FILE_PATH, params)) {
            promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to update measurement parameters file")));
            return promise->get_future();
        }

        promise->set_value(last_measurement_data);
        return promise->get_future();
    }

    auto timeoutMs = last_length_ms;
    std::thread([this, promise, timeoutMs]() {
        checkMeasurementCompletion(timeoutMs, promise);
    }).detach();
    return promise->get_future();
}

std::future<ISensorModule::FluorometerDetectorInfo> CanSensorModule::getFluorometerDetectorInfo() {
    return base.get<
        App_messages::Fluorometer::Detector_info_request, 
        App_messages::Fluorometer::Detector_info_response, 
        FluorometerDetectorInfo
    >([](App_messages::Fluorometer::Detector_info_response response){
        return FluorometerDetectorInfo{
            .peak_wavelength = response.wavelength,
            .sensitivity = response.sensitivity
            //.sampling_rate = response.type
        };
    }, 2000);
}

std::future<float> CanSensorModule::getFluorometerDetectorTemperature() {
    return base.get<
        App_messages::Fluorometer::Detector_temperature_request, 
        App_messages::Fluorometer::Detector_temperature_response, 
        float
    >([](App_messages::Fluorometer::Detector_temperature_response response){
        return response.temperature;
    }, 2000);
}

std::future<ISensorModule::FluorometerEmitorInfo> CanSensorModule::getFluorometerEmitorInfo() {
    return base.get<
        App_messages::Fluorometer::Emitor_info_request, 
        App_messages::Fluorometer::Emitor_info_response, 
        FluorometerEmitorInfo
    >([](App_messages::Fluorometer::Emitor_info_response response){
        return FluorometerEmitorInfo{
            .peak_wavelength = response.wavelength,
            .power_output = response.power_output
        };
    }, 2000);
}