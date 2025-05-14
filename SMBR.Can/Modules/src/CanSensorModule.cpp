#include "CanSensorModule.hpp"
#include "SMBR/Log.hpp"
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
#include "codes/messages/fluorometer/sample_request.hpp"
#include "codes/messages/fluorometer/sample_response.hpp"
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
#include "codes/messages/fluorometer/emitor_temperature_request.hpp"
#include "codes/messages/fluorometer/emitor_temperature_response.hpp"
#include "codes/messages/fluorometer/calibration_request.hpp"
#include "codes/messages/spectrophotometer/channel_count_request.hpp"
#include "codes/messages/spectrophotometer/channel_count_response.hpp"
#include "codes/messages/spectrophotometer/channel_info_request.hpp"
#include "codes/messages/spectrophotometer/channel_info_response.hpp"
#include "codes/messages/spectrophotometer/temperature_request.hpp"
#include "codes/messages/spectrophotometer/temperature_response.hpp"
#include "codes/messages/spectrophotometer/measurement_request.hpp"
#include "codes/messages/spectrophotometer/measurement_response.hpp"
#include "codes/messages/spectrophotometer/calibrate.hpp"
#include "codes/codes.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <fstream>
#include <sys/stat.h>     // for mkdir
#include <unistd.h>       // for access
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <ctime>

static const std::string PARAMS_FILE_PATH = "/data/api-server/fluorometer/measurement_params.json";

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
    }, default_timeout_ms);
}

std::future <float> CanSensorModule::getTopMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_measured_temperature_request,
        App_messages::Bottle_temperature::Top_measured_temperature_response,
        float
    >([](App_messages::Bottle_temperature::Top_measured_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <float> CanSensorModule::getBottomMeasuredTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_measured_temperature_request,
        App_messages::Bottle_temperature::Bottom_measured_temperature_response,
        float
    >([](App_messages::Bottle_temperature::Bottom_measured_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <float> CanSensorModule::getTopSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Top_sensor_temperature_request,
        App_messages::Bottle_temperature::Top_sensor_temperature_response,
        float
    >([](App_messages::Bottle_temperature::Top_sensor_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <float> CanSensorModule::getBottomSensorTemperature() {

    return base.get<
        App_messages::Bottle_temperature::Bottom_sensor_temperature_request,
        App_messages::Bottle_temperature::Bottom_sensor_temperature_response,
        float
    >([](App_messages::Bottle_temperature::Bottom_sensor_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future <bool> CanSensorModule::clearCustomText() {
    App_messages::Mini_OLED::Clear_custom_text r;
    return base.set(r);
}

std::future <bool> CanSensorModule::printCustomText(std::string text) {

    std::deque <CanRequest> tasks;

    {
        App_messages::Mini_OLED::Clear_custom_text r;
        tasks.push_back(base.createRequest(r));
    }

    for (size_t i = 0; i < text.length(); i += App_messages::Mini_OLED::Print_custom_text::max_length) {
        int remaining = text.length() - i;
        std::string chunk = "";
        if (remaining < App_messages::Mini_OLED::Print_custom_text::max_length) {
            chunk = text.substr(i, remaining);
        } else {
            chunk = text.substr(i, App_messages::Mini_OLED::Print_custom_text::max_length);
        }
        App_messages::Mini_OLED::Print_custom_text r(chunk);
        tasks.push_back(base.createRequest(r));
    }

    std::shared_ptr <std::promise <bool>> promise = std::make_shared<std::promise <bool>>();
    base.setMultiple(promise, tasks);
    return promise->get_future();
}

uint8_t CanSensorModule::CalculateMeasurementID(uint32_t api_id) {
    return (api_id % 15) + 1;
}

std::future<ISensorModule::FluorometerSample> CanSensorModule::takeFluorometerSingleSample(
    Fluorometer_config::Gain gain,
    float intensity
) {

    return base.get<
        App_messages::Fluorometer::Sample_request,
        App_messages::Fluorometer::Sample_response,
        FluorometerSample
    >(
        App_messages::Fluorometer::Sample_request(CalculateMeasurementID(sample_id), gain, intensity),
        [](App_messages::Fluorometer::Sample_response response) {
            FluorometerSample result;
            result.raw_value = response.sample_value;
            result.relative_value = response.Relative_value();
            result.absolute_value = response.Absolute_value();
            return result;
        },
        default_timeout_ms
    );
}

std::future<bool> CanSensorModule::isFluorometerOjipCaptureComplete() {
    return base.get<
        App_messages::Fluorometer::OJIP_completed_request,
        App_messages::Fluorometer::OJIP_completed_response,
        bool
    >([](App_messages::Fluorometer::OJIP_completed_response response){
        return response.completed;
    }, default_timeout_ms);
}

std::pair<bool, std::string> CanSensorModule::ensureDirectoryExists(const std::string& filePath) {
    try {
        Poco::Path path(filePath);
        if (path.isDirectory()) {
            return {true, ""};
        }

        Poco::File dir(path.parent());
        if (!dir.exists()) {
            dir.createDirectories();
        }
        return {dir.exists(), ""};
    } catch (const Poco::Exception& e) {
        std::string error = "Directory error [" + filePath + "]: " + e.displayText();
        return {false, error};
    }
}

std::pair<bool, std::string> CanSensorModule::readMeasurementParams(const std::string& filePath, MeasurementParams& params) {
    auto [dirSuccess, dirError] = ensureDirectoryExists(filePath);
    if (!dirSuccess) {
        return {false, dirError};
    }

    try {
        Poco::File file(filePath);
        if (!file.exists()) {
            return {false, "File does not exist: " + filePath};
        }
        if (file.getSize() == 0) {
            return {false, "File is empty: " + filePath};
        }

        std::ifstream input(filePath);
        if (!input.is_open()) {
            return {false, "Failed to open file: " + filePath};
        }

        Poco::JSON::Parser parser;
        auto result = parser.parse(input);
        input.close();

        auto object = result.extract<Poco::JSON::Object::Ptr>();
        if (!object) {
            return {false, "Invalid JSON structure in file: " + filePath};
        }

        static const std::vector<std::string> requiredFields = {
            "api_id", "samples", "length_ms", "timebase", "isRead"
        };

        for (const auto& field : requiredFields) {
            if (!object->has(field)) {
                return {false, "Missing field '" + field + "' in JSON file: " + filePath};
            }
        }

        try {
            params.api_id = object->getValue<int>("api_id");
            params.samples = object->getValue<int>("samples");
            params.length_ms = object->getValue<int>("length_ms");
            params.timebase = object->getValue<int>("timebase");
            params.isRead = object->getValue<bool>("isRead");
        } catch (const Poco::Exception& e) {
            return {false, "Type mismatch in JSON file [" + filePath + "]: " + e.displayText()};
        }

        return {true, ""};
    } catch (const Poco::Exception& e) {
        return {false, "JSON parse error [" + filePath + "]: " + e.displayText()};
    } catch (const std::exception& e) {
        return {false, "System error [" + filePath + "]: " + e.what()};
    } catch (...) {
        return {false, "Unknown error processing file: " + filePath};
    }
}

std::pair<bool, std::string> CanSensorModule::writeMeasurementParams(const std::string& filePath, const MeasurementParams& params) {
    auto [dirSuccess, dirError] = ensureDirectoryExists(filePath);
    if (!dirSuccess) {
        return {false, dirError};
    }

    const std::string tempPath = filePath + ".tmp";
    try {
        {
            std::ofstream output(tempPath, std::ios::binary);
            if (!output.is_open()) {
                return {false, "Failed to create temp file: " + tempPath};
            }

            Poco::JSON::Object json;
            json.set("api_id", params.api_id);
            json.set("samples", params.samples);
            json.set("length_ms", params.length_ms);
            json.set("timebase", params.timebase);
            json.set("isRead", params.isRead);

            Poco::JSON::Stringifier::stringify(json, output);
            output.close();

            if (output.fail()) {
                Poco::File(tempPath).remove();
                return {false, "Write error for temp file: " + tempPath};
            }
        }

        Poco::File tempFile(tempPath);
        tempFile.renameTo(filePath);

        return {true, ""};
    } catch (const Poco::Exception& e) {
        Poco::File(tempPath).remove();
        return {false, "Write operation failed [" + filePath + "]: " + e.displayText()};
    } catch (const std::exception& e) {
        Poco::File(tempPath).remove();
        return {false, "System error [" + filePath + "]: " + e.what()};
    } catch (...) {
        Poco::File(tempPath).remove();
        return {false, "Unknown error writing file: " + filePath};
    }
}


static std::string formatTime(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                time.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void CanSensorModule::sendCanRequest(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise) {
    App_messages::Fluorometer::OJIP_retrieve_request rawRequest;
    CanID requestId = BaseModule::createId(rawRequest.Type(), Codes::Module::Sensor_module, Codes::Instance::Exclusive, false);

    RequestData requestData(requestId, rawRequest.Export_data());
    ResponseInfo responseInfo;
    responseInfo.timeoutMs = timeoutMs;

    auto context = std::make_shared<ProcessingContext>();
    context->startTime = std::chrono::steady_clock::now();
    auto startSystemTime = std::chrono::system_clock::now();

    LNOTICE("ojipRetrieving") << "[" << formatTime(startSystemTime) << "] Starting OJIP measurement retrieval, timeout: "
                             << timeoutMs << "ms" << LE;

    responseInfo.acceptFunction = [](const ResponseData& response) {
        App_messages::Fluorometer::Data_sample rawResponse;
        return response.id.messageType() == static_cast<uint16_t>(rawResponse.Type());
    };

    responseInfo.isDoneFunction = [this, context](const std::vector<ResponseData>& responses) {
        context->responses = responses;

        if (!responses.empty() && !context->firstSampleLogged) {
            context->firstSampleTime = std::chrono::steady_clock::now();
            auto firstSampleSystemTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                context->firstSampleTime - context->startTime).count();

            LNOTICE("ojipRetrieving") << "[" << formatTime(firstSampleSystemTime) << "] First sample received after "
                                     << duration << "ms" << LE;
            context->firstSampleLogged = true;
        }

        if (!responses.empty()) {
            context->lastSampleTime = std::chrono::steady_clock::now();
        }

        return responses.size() >= static_cast<size_t>(last_required_samples);
    };

    responseInfo.successOnTimeout = true;

    CanRequest canRequest(requestData, responseInfo);

    channel->send("ojip", canRequest, [this, promise, context, startSystemTime](ICanChannel::Response response) {
        if (!context->processed) {
            for (const auto& rr : context->responses) {
                auto dataCopy = rr.data;
                App_messages::Fluorometer::Data_sample sampleResponse;
                if (sampleResponse.Interpret_data(dataCopy) && sampleResponse.Measurement_id() == CalculateMeasurementID(last_api_id)) {
                    context->receivedSamples++;

                    if (!context->metadataLoaded) {
                        context->result.detector_gain = sampleResponse.gain;
                        context->result.emitor_intensity = sampleResponse.Emitor_intensity();
                        context->metadataLoaded = true;
                    }

                    ISensorModule::FluorometerSample sample;
                    sample.time_ms = sampleResponse.Time_ms();
                    sample.raw_value = sampleResponse.sample_value;
                    sample.relative_value = sampleResponse.Relative_value();
                    sample.absolute_value = sampleResponse.Absolute_value();
                    context->result.samples.push_back(sample);
                    context->result.measurement_id = last_api_id;

                    if (sample.raw_value > 4000) {
                        context->isSaturated = true;
                    }
                }
            }
            context->processed = true;
        }

        auto endSystemTime = std::chrono::system_clock::now();
        auto endTime = std::chrono::steady_clock::now();

        if (!context->responses.empty()) {
            auto samplesDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                context->lastSampleTime - context->firstSampleTime).count();

            LNOTICE("ojipRetrieving") << "[" << formatTime(endSystemTime) << "] Last sample received, total samples: "
                                     << context->receivedSamples << ", collection duration: " << samplesDuration << "ms"<< LE;
        }

        if (response.status == CanRequestStatus::Success || response.status == CanRequestStatus::Timeout) {
            if (context->result.samples.empty()) {
                promise->set_exception(std::make_exception_ptr(
                    std::runtime_error("No measurement data available on the device")));
                return;
            }

            std::sort(context->result.samples.begin(), context->result.samples.end(),
                [](const FluorometerSample& a, const FluorometerSample& b) {
                    return a.time_ms < b.time_ms;
                });

            context->result.timebase = last_timebase;
            context->result.length_ms = last_length_ms;
            context->result.required_samples = last_required_samples;
            context->result.captured_samples = static_cast<int16_t>(context->result.samples.size());
            context->result.missing_samples = context->result.required_samples - context->result.captured_samples;
            context->result.read = isRead;
            context->result.saturated = context->isSaturated;

            last_measurement_data = context->result;
            promise->set_value(context->result);
        } else {
            promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to send request")));
        }
    });
}

void CanSensorModule::checkMeasurementCompletion(uint32_t timeoutMs, std::shared_ptr<std::promise<ISensorModule::FluorometerOjipData>> promise) {
    try {
        int attempts = 0;
        const int maxAttempts = 20;
        const std::chrono::milliseconds delay(100);
        while (attempts < maxAttempts) {
            try {
                auto future = isFluorometerOjipCaptureComplete();
                if (future.get()) {
                    sendCanRequest(timeoutMs, promise);
                    return;
                }
            } catch (const std::exception& e) {
                //std::cerr << "Error during fluorometer capture check: " << e.what() << std::endl;
            }

            std::this_thread::sleep_for(delay);
            attempts++;
        }

        promise->set_exception(std::make_exception_ptr(
            std::runtime_error("Measurement did not complete within the expected time")
        ));
    } catch (const std::exception& e) {
        promise->set_exception(std::make_exception_ptr(e));
    } catch (...) {
        promise->set_exception(std::make_exception_ptr(
            std::runtime_error("Unknown error occurred during measurement completion check")
        ));
    }
}

std::future<ISensorModule::FluorometerOjipData> CanSensorModule::captureFluorometerOjip(const ISensorModule::FluorometerInput& input) {
    uint32_t api_id;
    uint8_t measurement_id;
    auto startTime = std::chrono::system_clock::now();
    LNOTICE("ojipCapture") << "[" << formatTime(startTime)
                         << "] Starting OJIP capture process" << LE;

    MeasurementParams params;
    auto [success, error] = readMeasurementParams(PARAMS_FILE_PATH, params);
    if (success) {
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
    last_timebase = input.sample_timing;

    params.api_id = api_id;
    params.samples = input.sample_count;
    params.length_ms = input.length_ms;
    params.timebase = static_cast<int>(input.sample_timing);
    params.isRead = isRead;

    auto [writeSuccess, writeError] = writeMeasurementParams(PARAMS_FILE_PATH, params);
    if (!writeSuccess) {
        throw std::runtime_error("Failed to write measurement parameters: " + writeError);
    }

    auto timeoutMs = input.length_ms;
    auto sampleTimeoutMs = 1000 + (5 * input.sample_count);
    last_required_samples = input.sample_count;
    last_length_ms = input.length_ms;

    App_messages::Fluorometer::OJIP_capture_request r{
        measurement_id,
        input.detector_gain,
        input.sample_timing,
        input.emitor_intensity,
        input.length_ms,
        input.sample_count
    };

    LNOTICE("ojipCapture") << "[" << formatTime(std::chrono::system_clock::now())
                        << "] Sending capture request" << LE;

    auto succes = base.set(r);
    if (succes.get()) {
        auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();

        std::thread([this, promise, timeoutMs, sampleTimeoutMs]() {
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
                checkMeasurementCompletion(sampleTimeoutMs, promise);
            } catch (const std::exception& e) {
                promise->set_exception(std::make_exception_ptr(e));
            }
        }).detach();

        return promise->get_future();
    } else {
        auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();
        promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to start OJIP capture")));
        return promise->get_future();
    }
}

std::future<ISensorModule::FluorometerOjipData> CanSensorModule::retrieveLastFluorometerOjipData() {
    auto promise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();

    if (last_api_id == 0) {
        MeasurementParams params;
        auto [success, error] = readMeasurementParams(PARAMS_FILE_PATH, params);
        if (success) {
            last_api_id = params.api_id;
            last_required_samples = params.samples;
            last_length_ms = params.length_ms;
            last_timebase = static_cast<Fluorometer_config::Timing>(params.timebase);
            isRead = params.isRead;
        } else {
            promise->set_exception(std::make_exception_ptr(std::runtime_error(error)));
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
        MeasurementParams params = {last_api_id, last_required_samples, last_length_ms,
                                  static_cast<int>(last_timebase), isRead};
        auto [writeSuccess, writeError] = writeMeasurementParams(PARAMS_FILE_PATH, params);
        if (!writeSuccess) {
            promise->set_exception(std::make_exception_ptr(
                std::runtime_error("Failed to update measurement parameters file: " + writeError)));
            return promise->get_future();
        }

        promise->set_value(last_measurement_data);
        return promise->get_future();
    }

    auto sampleTimeoutMs = 2 * last_required_samples;
    auto threadPromise = std::make_shared<std::promise<ISensorModule::FluorometerOjipData>>();
    auto future = threadPromise->get_future();

    LINFO("Thread") << "retrieveLastFluorometerOjipData new thread to be started" << LE;

    std::thread([this, threadPromise, sampleTimeoutMs]() {
            try {
                checkMeasurementCompletion(sampleTimeoutMs, threadPromise);
            } catch (const std::exception& e) {
                threadPromise->set_exception(std::make_exception_ptr(e));
            }
    }).detach();

    return std::async(std::launch::deferred, [this, future = std::move(future)]() mutable {
        ISensorModule::FluorometerOjipData result = future.get();

        if (!isRead) {
            isRead = true;
            MeasurementParams params = {last_api_id, last_required_samples, last_length_ms,
                                  static_cast<int>(last_timebase), isRead};
            writeMeasurementParams(PARAMS_FILE_PATH, params);
        }
        return result;
    });
}

std::future<ISensorModule::FluorometerDetectorInfo> CanSensorModule::getFluorometerDetectorInfo() {
    return base.get<
        App_messages::Fluorometer::Detector_info_request,
        App_messages::Fluorometer::Detector_info_response,
        FluorometerDetectorInfo
    >([](App_messages::Fluorometer::Detector_info_response response){
        return FluorometerDetectorInfo{
            .peak_wavelength = response.wavelength,
            .sensitivity = response.sensitivity,
            .sampling_rate = response.sampling_rate
        };
    }, default_timeout_ms);
}

std::future<float> CanSensorModule::getFluorometerDetectorTemperature() {
    return base.get<
        App_messages::Fluorometer::Detector_temperature_request,
        App_messages::Fluorometer::Detector_temperature_response,
        float
    >([](App_messages::Fluorometer::Detector_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
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
    }, default_timeout_ms);
}

std::future<float> CanSensorModule::getFluorometerEmitorTemperature() {
    return base.get<
        App_messages::Fluorometer::Emitor_temperature_request,
        App_messages::Fluorometer::Emitor_temperature_response,
        float
    >([](App_messages::Fluorometer::Emitor_temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future<bool> CanSensorModule::calibrateFluorometer() {
    App_messages::Fluorometer::Calibration_request r;
    return base.set(r);
}

std::future<int8_t> CanSensorModule::getSpectrophotometerChannels() {
    return base.get<
        App_messages::Spectrophotometer::Channel_count_request,
        App_messages::Spectrophotometer::Channel_count_response,
        int8_t
    >([](App_messages::Spectrophotometer::Channel_count_response response){
        return response.channel_count;
    }, default_timeout_ms);
}

std::future<ISensorModule::SpectroChannelInfo> CanSensorModule::getSpectrophotometerChannelInfo(int8_t channelNumber) {
    return base.getByChannel<
        App_messages::Spectrophotometer::Channel_info_request,
        App_messages::Spectrophotometer::Channel_info_response,
        ISensorModule::SpectroChannelInfo
    >(channelNumber, default_timeout_ms, [channelNumber](const App_messages::Spectrophotometer::Channel_info_response& response) {
        ISensorModule::SpectroChannelInfo result;
        result.channel = response.channel;
        result.peak_wavelength = response.central_wavelength;
        result.half_intensity_peak_width = response.half_sensitivity_width;
        return result;
    });
}

std::future<ISensorModule::SpectroChannelMeasurement> CanSensorModule::measureSpectrophotometerChannel(int8_t channelNumber) {
    return base.getByChannel<
        App_messages::Spectrophotometer::Measurement_request,
        App_messages::Spectrophotometer::Measurement_response,
        ISensorModule::SpectroChannelMeasurement
    >(channelNumber, default_timeout_ms, [channelNumber](const App_messages::Spectrophotometer::Measurement_response& response) {
        ISensorModule::SpectroChannelMeasurement result;
        result.channel = response.channel;
        result.relative_value = response.relative_value;
        result.absolute_value = response.absolute_value;
        return result;
    });
}

std::future<float> CanSensorModule::getSpectrophotometerEmitorTemperature() {
    return base.get<
        App_messages::Spectrophotometer::Temperature_request,
        App_messages::Spectrophotometer::Temperature_response,
        float
    >([](App_messages::Spectrophotometer::Temperature_response response){
        return response.temperature;
    }, default_timeout_ms);
}

std::future<bool> CanSensorModule::calibrateSpectrophotometer() {
    App_messages::Spectrophotometer::Calibrate r;
    return base.set(r);
}
